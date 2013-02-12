#include "TGame.h"


namespace TLGame
{
	TRef AllocPlayerRef()
	{
		static TRef LastRef( 0 );
		LastRef.mRef++;
		return LastRef;
	}
}


TPlayerMeta::TPlayerMeta(const TString& Name,const ofColour& Colour) :
	mName	( Name ),
	mColour	( Colour ),
	mRef	( TLGame::AllocPlayerRef() )
{
}


TGame::TGame(const TPlayerMeta& Player1,const TPlayerMeta& Player2)
{
	mPlayers.PushBack( Player1 );
	mPlayers.PushBack( Player2 );

	mCurrentPlayer = mPlayers[0].mRef;
}

bool TGame::Init()
{
	//	create background stars
	mWorld.CreateActor<TActorStars>();

	BufferArray<vec2f,2> PlayerPositions;
	PlayerPositions.PushBack( vec2f(-WORLD_WIDTH/4,0) );
	PlayerPositions.PushBack( vec2f(WORLD_WIDTH/4,0) );


	//	init world with some player entities
	for ( int p=0;	p<mPlayers.GetSize();	p++ )
	{
		auto& Player = mPlayers[p];

		//	create main death star!
		Player.mDeathStar = mWorld.CreateActor<TActorDeathStar>();
		Player.mDeathStar->mMaterial.mColour = Player.mColour;
		Player.mDeathStar->SetPosition( PlayerPositions[p] );
	}

	//	create sentrys/weapons 
	//	todo: auto-managed by death star!
	for ( int p=0;	p<mPlayers.GetSize();	p++ )
	{
		auto& Player = mPlayers[p];
		auto& DeathStar = *Player.mDeathStar;
	
		for ( int i=0;	i<SENTRY_COUNT;	i++ )
		{
			float AngleDeg = 360.f * ( static_cast<float>(i) / static_cast<float>( SENTRY_COUNT ) );
			float AngleRad = ofDegToRad( AngleDeg );
		
			//	position on edge of death star
			ofShapeCircle2 DeathStarShape = DeathStar.GetCollisionShape().mCircle;
			ofShapeCircle2 SentryPos( SENTRY_RADIUS );
			SentryPos.mPosition = vec2f( sinf( AngleRad ), cosf( AngleRad ) );
			SentryPos.mPosition *= SentryPos.mRadius + DeathStarShape.mRadius;
			SentryPos.mPosition += DeathStarShape.mPosition;

			Player.mSentrys.PushBack( mWorld.CreateActor<TActorSentry>( SentryPos, Player.mColour ) );
		}
	}

	//	init camera
	mCamera.enableOrtho();
	mCamera.setFarClip(1000.f);
	mCamera.setNearClip(0.f);

//	mCamera.disableMouseInput();
	float Distance = mCamera.getImagePlaneDistance( ofGetCurrentViewport() );
	mCamera.setPosition( Distance * mCamera.getZAxis() );

	return true;
}

void TGame::Update(float TimeStep)
{
	//	update post-input things
	UpdateDrags();

	//	update entities
	mWorld.Update( TimeStep );

	//	do collisoin before packets then it all happens in the same frame :)
	UpdateCollisions();

	//	process game packets (logic basically)
	UpdateGamePackets();

}

void TGame::Render(float TimeStep)
{
	TRenderSceneScope Scene(__FUNCTION__);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	//ofEnableAlphaBlending();

	mOrthoViewport = ofGetCurrentViewport();
	ofRectangle& OrthoViewport = mOrthoViewport;
	float Ratio = OrthoViewport.height / OrthoViewport.width;
	OrthoViewport.width = WORLD_WIDTH;
	OrthoViewport.height = OrthoViewport.width * Ratio;
	
	OrthoViewport.x -= OrthoViewport.width/2;
	OrthoViewport.y -= OrthoViewport.height/2;
	
	OrthoViewport.translate( mGameCamera.mScroll.x, mGameCamera.mScroll.y );
	mCamera.begin( ofGetCurrentViewport(), OrthoViewport );

	//	render scene
	RenderWorld(TimeStep);

	//	render hud
	RenderHud(TimeStep);


	mCamera.end();
}


void TGame::RenderWorld(float TimeStep)
{
	//	render game background
	ofClear( ofColour(30,30,30) );

	//	render world entities
	static bool RenderCollision = false;
	TRenderSettings RenderSettings;
	RenderSettings.mMode = RenderCollision ? TRenderMode::Collision : TRenderMode::Colour;
	mWorld.Render( TimeStep, RenderSettings );
}


void TGame::RenderHud(float TimeStep)
{
	//	render player health's
	for ( int p=0;	p<mPlayers.GetSize();	p++ )
	{
		TRenderSceneScope Scene(__FUNCTION__);
		glTranslatef( 0, 0, GUI_Z );

		auto& Player = mPlayers[p];

		TString String;
		String << Player.mHealth << "%";
		std::string StdString( static_cast<const char*>( String ) );

		//	player -> screen
		vec3f WorldPos = Player.mDeathStar->GetPosition3();

		vec2f ScreenPos = WorldPos;

		//	center text
		vec2f TextSize( TRender::Font.stringWidth(StdString), TRender::Font.stringHeight(StdString) );
		ScreenPos.x -= TextSize.x / 2.f;
		ScreenPos.y += TextSize.y / 2.f;

		TRender::Font.drawString( StdString, ScreenPos.x, ScreenPos.y );
	}
}


void TGame::UpdateInput(SoyInput& Input)
{
	Input.PopLock();
	//	collect all the drags
	while ( true )
	{
		SoyGesture Gesture = Input.PopGesture();
		if ( !Gesture.IsValid() )
			break;

		//	ignore if not lmb
		if ( Gesture.GetButton() != SoyButton::Left )
			continue;

		
		//	get current drag
		TPlayerDrag* pCurrentDrag = NULL;
		if ( !mPendingDrags.IsEmpty() )
		{
			pCurrentDrag = &mPendingDrags.GetBack();
			//	if current drag is finished, ignore it
			if ( pCurrentDrag->mFinished )
				pCurrentDrag = NULL;
		}

		//	if gesture is a release, end the current drag if there is one...
		if ( Gesture.IsUp() && pCurrentDrag )
		{
			pCurrentDrag->mFinished = true;
		}
		else if ( !pCurrentDrag && Gesture.IsDown() )
		{
			//	new drag
			TryNewDrag( Gesture );
		}
		else if ( pCurrentDrag && Gesture.IsDown() )
		{
			//	update end point of current drag
			pCurrentDrag->mDragTo = ScreenToWorld( Gesture.mPath.GetBack(), GUI_Z );
		}
		
	}
	Input.PopUnlock();
}


bool TGame::TryNewDrag(const SoyGesture& Gesture)
{
	//	find a player's sentry we're closest to
	TPlayer* pPlayer = GetPlayer( GetCurrentPlayer() );
	if ( !pPlayer )
		return false;
	auto& Player = *pPlayer;

	//	see where this screen shape hits a sentry
	ofShapeCircle2 FingerShape( Gesture.mPath[0], FINGER_SCREEN_SIZE );

	int NearestSentry = -1;
	float NearestSentryDistanceSq = 0.f;
	
	for ( int i=0;	i<Player.mSentrys.GetSize();	i++ )
	{
		auto& Sentry = *Player.mSentrys[i];

		//	get screen-collision shape of sentry
		ofShapeCircle2 SentryWorldShape = Sentry.GetCollisionShape().mCircle;
		ofShapeCircle2 SentryShape = WorldToScreen( SentryWorldShape, Sentry.GetPosition3().z );

		//	get distance from finger...
		TIntersection2 Intersection = FingerShape.GetIntersection( SentryShape );
		if ( !Intersection )
			continue;

		//	is better result?
		if ( NearestSentry == -1 || Intersection.mDistanceSq < NearestSentryDistanceSq )
		{
			NearestSentry = i;
			NearestSentryDistanceSq = Intersection.mDistanceSq;
		}
	}		

	//	none near the finger!
	if ( NearestSentry == -1 )
		return false;

	auto& Sentry = *Player.mSentrys[NearestSentry];

	//	create a drag from that sentry
	TPlayerDrag* pCurrentDrag = NULL;
	pCurrentDrag = &mPendingDrags.PushBack();
	pCurrentDrag->mPlayer = GetCurrentPlayer();
	pCurrentDrag->mSentry = Sentry.GetRef();
	pCurrentDrag->mDragTo = ScreenToWorld( Gesture.mPath.GetBack(), GUI_Z );

	return true;
}


void TGame::OnDragStarted(TPlayerDrag& Drag)
{
	//	create actor
	if ( !Drag.mActor )
	{
		Drag.mActor = mWorld.CreateActor<TActorDrag>();
	}
}

void TGame::OnDragEnded(TPlayerDrag& Drag)
{
	//	clean up drag graphics 
	mWorld.DestroyActor( *Drag.mActor );
	Drag.mActor = NULL;

	//	launch rocket if there was enough of a drag.
	ofLine2 DragLine = Drag.GetLine( mWorld );
	if ( DragLine.GetLength() < 3.f )
		return;

	TGamePacket_FireRocket Packet;
	Packet.mFiringLine = DragLine;
	Packet.mPlayerRef = Drag.mPlayer;
	mGamePackets.PushPacket( Packet );
}

void TGame::UpdateDrag(TPlayerDrag& Drag)
{
	//	new drag!
	if ( !Drag.mActor )
	{
		OnDragStarted( Drag );
	}

	if ( !Drag.mActor )
	{
		assert( Drag.mActor );
		return;
	}
	
	//	update actor graphics/sound etc
	Drag.mActor->SetLine( Drag.GetLine( mWorld ) );
}

void TGame::UpdateDrags()
{
	//	update the drag gestures
	for ( int i=mPendingDrags.GetSize()-1;	i>=0;	i-- )
	{
		auto& Drag = mPendingDrags[i];

		//	handle finsihed ones
		if ( Drag.mFinished )
		{
			OnDragEnded( Drag );
			mPendingDrags.RemoveBlock( i,1 );
			continue;
		}

		UpdateDrag( Drag );
	}

}



vec2f TGame::ScreenToWorld(const vec2f& Screen2,float Z)
{
	//	scale the rendertarget pos to a pos between 0..100 (100 being GetOrthoRange - and scale is always based on width hence why both are /wh.x)
	ofRectangle Viewport = ofGetCurrentViewport();

	//	normalise screen coords
	float ScreenNormalx = Screen2.x / Viewport.width;
	float ScreenNormaly = Screen2.y  / Viewport.height;

	//	expand to ortho range
	float OrthoX = ScreenNormalx * mOrthoViewport.width;
	float OrthoY = ScreenNormaly * mOrthoViewport.height;
	OrthoX += mOrthoViewport.x;
	OrthoY += mOrthoViewport.y;
	
	/*
	//get inverse camera matrix
	ofMatrix4x4 inverseCamera;
	inverseCamera.makeInvertOf(getModelViewProjectionMatrix(viewport));

	//convert camera to world
	vec3f World3( CameraXYZ * inverseCamera );
	
	return vec2f( World3.x, World3.y );
	*/
	return vec2f( OrthoX, OrthoY );
}


ofShapeCircle2 TGame::WorldToScreen(const ofShapeCircle2& Shape,float Z)
{
	//	todo: proper inverse conversion... 
	vec2f Screen2 = WorldToScreen( vec3f( Shape.mPosition.x, Shape.mPosition.y, Z ) );

	float ScreenRadius = Shape.mRadius;

	//	todo: apply scale/zoom of camera
	return ofShapeCircle2( Screen2, ScreenRadius );
}


vec2f TGame::WorldToScreen(const vec3f& World3)
{
	ofRectangle Viewport = ofGetCurrentViewport();
//	ofVec3f CameraXYZ = World3 * mCamera.getModelViewProjectionMatrix();
	ofVec3f CameraXYZ = World3;

	//	view to screen UV
	vec2f Screen2;
	Screen2.x = CameraXYZ.x - mOrthoViewport.x;
	Screen2.y = CameraXYZ.y - mOrthoViewport.y;
	Screen2.x /= mOrthoViewport.width;
	Screen2.y /= mOrthoViewport.height;
	
	//	stretch to real screen
	Screen2.x *= Viewport.width;
	Screen2.y *= Viewport.height;

	return Screen2;
	/*
	ofVec3f Camera3 = World3 * getModelViewProjectionMatrix();
	ofVec3f Screen3;

	Screen3.x = (CameraXYZ.x + 1.0f) / 2.0f * viewport.width + viewport.x;
	Screen3.y = (1.0f - CameraXYZ.y) / 2.0f * viewport.height + viewport.y;

	/*
	//get inverse camera matrix
	ofMatrix4x4 inverseCamera;
	inverseCamera.makeInvertOf(getModelViewProjectionMatrix(viewport));

	//convert camera to world
	vec3f World3( CameraXYZ * inverseCamera );
	
	return vec2f( World3.x, World3.y );
	*/
	//return vec2f( OrthoX, OrthoY );
}

void TGame::UpdateGamePackets()
{
	while ( true )
	{
		TGamePacket* pPacket = mGamePackets.PopPacket();
		if ( !pPacket ) 
			break;

		if ( !OnPacket( *pPacket ) )
		{
			//	unhandled packet
		}

		delete pPacket;
	}
}

bool TGame::OnPacket(TGamePacket& Packet)
{
	switch ( Packet.GetType() )
	{
	case TGamePackets::FireRocket:
		OnPacket_FireRocket( static_cast<TGamePacket_FireRocket&>( Packet ) );
		return true;
	case TGamePackets::Collision_RocketPlayer:
		OnPacket_Collision( static_cast<TGamePacket_CollisionRocketPlayer&>( Packet ) );
		return true;
	case TGamePackets::Collision_RocketSentry:
		OnPacket_Collision( static_cast<TGamePacket_CollisionRocketSentry&>( Packet ) );
		return true;
	}

	return false;
}

void TGame::OnPacket_FireRocket(TGamePacket_FireRocket& Packet)
{
	//	create rocket actor
	mWorld.CreateActor<TActorRocket>( Packet.mFiringLine, Packet.mPlayerRef );
}


void TGame::OnPacket_Collision(TGamePacket_CollisionRocketSentry& Packet)
{
	//	actor A is the rocket
	mWorld.CreateActor<TActorExplosion>( Packet.mIntersection.mCollisionPointB );
	mWorld.DestroyActor( Packet.mActorRocket );

	//	destroy the sentry we just hit
	mWorld.DestroyActor( Packet.mActorSentry );
}

void TGame::OnPacket_Collision(TGamePacket_CollisionRocketPlayer& Packet)
{
	//	actor A is the rocket
	mWorld.CreateActor<TActorExplosion>( Packet.mIntersection.mCollisionPointB );
	mWorld.DestroyActor( Packet.mActorRocket );

	//	find the player
	TPlayer* pPlayer = GetPlayer( Packet.mActorDeathStar );
	if ( pPlayer )
	{
		pPlayer->mHealth -= 10;
	}
}

TPlayer* TGame::GetPlayer(TActorRef ActorRef)
{
	//	grab the actor...
	TActor& Actor = mWorld.GetActor( ActorRef );

	for ( int p=0;	p<mPlayers.GetSize();	p++ )
	{
		auto& Player = mPlayers[p];

		//	match an actor a player
		if ( *Player.mDeathStar == Actor )
			return &Player;

		for ( int s=0;	s<Player.mSentrys.GetSize();	s++ )
			if ( *Player.mSentrys[s] == Actor )
				return &Player;
	}

	return NULL;
}



void TGame::UpdateCollisions()
{
	while ( true )
	{
		TCollision Collision = mWorld.PopCollision();
		if ( !Collision.IsValid() )
			break;

		//	grab the actors for the functions
		TActor& ActorA = mWorld.GetActor( Collision.mActorA );
		TActor& ActorB = mWorld.GetActor( Collision.mActorB );

		if ( HandleCollision<TActorRocket,TActorDeathStar>( Collision, ActorA, ActorB ) )	continue;
		if ( HandleCollision<TActorRocket,TActorSentry>( Collision, ActorA, ActorB ) )	continue;

		//	unhandled
		OnCollision( Collision, ActorA, ActorB );
	}
}


void TGame::OnCollision(const TCollision& Collision,TActorRocket& ActorA,TActorDeathStar& ActorB)
{
	//	if the rocket hits it's owner player, ignore the collision
	TPlayer* pPlayer = GetPlayer( ActorB.GetRef() );
	assert( pPlayer );
	if ( pPlayer && pPlayer->mRef == ActorA.mPlayerRef )
		return;

	//	kablammo!
	TGamePacket_CollisionRocketPlayer Packet;
	Packet.mActorRocket = ActorA;
	Packet.mActorDeathStar = ActorB;
	Packet.mIntersection = Collision.mIntersection;
	mGamePackets.PushPacket( Packet );
}


void TGame::OnCollision(const TCollision& Collision,TActorRocket& ActorA,TActorSentry& ActorB)
{
	//	if the rocket hits it's owner player, ignore the collision
	TPlayer* pPlayer = GetPlayer( ActorB.GetRef() );
	assert( pPlayer );
	if ( pPlayer && pPlayer->mRef == ActorA.mPlayerRef )
		return;

	//	kablammo!
	TGamePacket_CollisionRocketSentry Packet;
	Packet.mActorRocket = ActorA;
	Packet.mActorSentry = ActorB;
	Packet.mIntersection = Collision.mIntersection;
	mGamePackets.PushPacket( Packet );
}

void TGame::OnCollision(const TCollision& Collision,TActor& ActorA,TActor& ActorB)
{
	
}

ofLine2 TPlayerDrag::GetLine(TWorld& World) const
{
	//	get actor
	auto& Sentry = World.GetActor( mSentry );
	ofShapeCircle2 SentryShape = Sentry.GetCollisionShape().mCircle;
	return ofLine2( SentryShape.mPosition, mDragTo );
}

