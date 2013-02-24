#include "TGame.h"



vec2f TPlayerDrag::GetWorldDragTo(TGame& Game,float z) const
{
	return Game.ScreenToWorld( mScreenDragTo, z );
}




TGame::TGame(const TGameMeta& GameMeta) :
	mGameState	( TGameState::PlayerOneTurn ),
	mGameMeta	( GameMeta )
{
	mPlayers.PushBack( GameMeta.mPlayerA );
	mPlayers.PushBack( GameMeta.mPlayerB );
}




bool TGame::Init()
{
	//	create background stars
	if ( ENABLE_STARS )
		mWorld.CreateActor<TActorStars>();

	BufferArray<vec2f,2> PlayerPositions;
	PlayerPositions.PushBack( vec2f(-WORLD_WIDTH/4,0) );
	PlayerPositions.PushBack( vec2f(WORLD_WIDTH/4,0) );
	BufferArray<vec3f,ASTEROID_COUNT> AsteroidPositionRad;	//	z is radius
	for ( int i=0;	i<AsteroidPositionRad.MaxSize();	i++ )
	{
		vec2f Pos( ofRandom( -80, 80 ), ofRandom( -600, 600 ) );
		float Rad( ofRandom( 10, 40 ) );
		AsteroidPositionRad.PushBack( vec3f( Pos.x, Pos.y, Rad ) );
	}

	//	init world with some player entities
	for ( int p=0;	p<mPlayers.GetSize();	p++ )
	{
		auto& Player = mPlayers[p];

		//	create main death star!
		TActorMeta Meta;
		Meta.mOwnerPlayer = Player.mRef;
		Meta.mColour = Player.mColour;
		Meta.mPosition = PlayerPositions[p];
		Player.mDeathStar = mWorld.CreateActor<TActorDeathStar>( Meta );
	}

	//	create some asteroids
	for ( int i=0;	i<AsteroidPositionRad.GetSize();	i++ )
	{
		auto& xyrad = AsteroidPositionRad[i];
		TActorMeta Meta;
		Meta.mColour = ofColour( 190, 150, 80 );
		Meta.mPosition = xyrad.xy();
		float Radius = xyrad.z;
		auto Asteroid = mWorld.CreateActor<TActorAsteroid>( Meta, Radius );
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
	mWorld.UpdateCollisions( *this );

	//	process game packets (real logic from simulation etc)
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
	//	render world entities
	TRenderSettings RenderSettings;
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
		vec3f WorldPos = Player.mDeathStar->GetWorldPosition3();

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
			if ( pCurrentDrag->IsFinished() )
				pCurrentDrag = NULL;
		}

		//	if gesture is a release, end the current drag if there is one...
		if ( Gesture.IsUp() && pCurrentDrag )
		{
			pCurrentDrag->SetFinished();
		}
		else if ( !pCurrentDrag && Gesture.IsDown() )
		{
			//	new drag
			TryNewDrag( Gesture );
		}
		else if ( pCurrentDrag && Gesture.IsDown() )
		{
			//	update end point of current drag
			pCurrentDrag->SetScreenDragTo( Gesture.mPath.GetBack() );
		}
		
	}
	Input.PopUnlock();
}


bool TGame::TryNewDrag(const SoyGesture& Gesture)
{
	//	find a player's sentry we're closest to
	TRef CurrentPlayer = GetCurrentControlPlayer();
	TPlayer* pPlayer = GetPlayer( CurrentPlayer );
	if ( !pPlayer )
		return false;
	auto& Player = *pPlayer;

	//	see where this screen shape hits a sentry
	ofShapeCircle2 FingerShape( Gesture.mPath[0], FINGER_SCREEN_SIZE );

	int NearestSentry = -1;
	float NearestSentryDistanceSq = 0.f;
	
	Array<TActorSentry*> Sentrys;
	if ( Player.mDeathStar )
		Player.mDeathStar->GetSentrys( mWorld, Sentrys );

	for ( int i=0;	i<Sentrys.GetSize();	i++ )
	{
		auto& Sentry = *Sentrys[i];

		//	get screen-collision shape of sentry
		ofShapeCircle2 SentryWorldShape = Sentry.GetWorldCollisionShape().mCircle;
		ofShapeCircle2 SentryShape = WorldToScreen( SentryWorldShape, Sentry.GetWorldPosition3().z );

		//	get distance from finger...
		TIntersection Intersection = FingerShape.GetIntersection( SentryShape );
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

	auto& Sentry = *Sentrys[NearestSentry];

	//	create a drag from that sentry
	TPlayerDrag* pCurrentDrag = NULL;
	pCurrentDrag = &mPendingDrags.PushBack();
	pCurrentDrag->mPlayer = CurrentPlayer;
	pCurrentDrag->mSentry = Sentry.GetRef();
	pCurrentDrag->SetScreenDragTo( Gesture.mPath.GetBack() );
	return true;
}


void TGame::OnDragStarted(TPlayerDrag& Drag)
{
	auto* pSentry = mWorld.GetActor<TActorSentry>( Drag.mSentry );
	if ( !pSentry )
	{
		assert( pSentry );
		return;
	}
	auto& Sentry = *pSentry;

	//	set sentry as active
	Sentry.SetState( TActorSentryState::Active );

	switch ( Sentry.GetType() )
	{
	//	if the sentry is a rocket, we create a UI drag
	case TActors::SentryRocket:
		if ( !Drag.mActorDrag )
			Drag.mActorDrag = mWorld.CreateActor<TActorDrag>();
		break;
	
	//	create UI for drag
	case TActors::SentryMissile:
		if ( !Drag.mActorDragPath )
			Drag.mActorDragPath = mWorld.CreateActor<TActorDragPath>();
		break;
	}

	Drag.mState = TPlayerDragState::Active;
}

void TGame::OnDragEnded(TPlayerDrag& Drag)
{
	Drag.mState = TPlayerDragState::Finished;	//	probably already set


	auto* pSentry = mWorld.GetActor<TActorSentry>( Drag.mSentry );
	if ( !pSentry )
	{
		assert( pSentry );
		return;
	}
	auto& Sentry = *pSentry;
	Sentry.SetState( TActorSentryState::Inactive );

	if ( Sentry.GetType() == TActors::SentryRocket )
	{
		//	launch rocket if there was enough of a drag.
		//	gr: should be a 2D test?
		ofLine2 DragLine = Drag.GetWorldDragLine( mWorld, *this );
		if ( DragLine.GetLength() < 3.f )
			return;

		TGamePacket_FireRocket Packet;
		Packet.mFiringLine = DragLine;
		Packet.mPlayerRef = Drag.mPlayer;
		mGamePackets.PushPacket( Packet );
	}

	if ( Sentry.GetType() == TActors::SentryMissile )
	{
		//	launch rocket if there was enough of a path
		//	gr: shouldn't really mix this... the path should be in the drag, and not rely on the actor...
		ofShapePath2 Path;
		auto* pDragPath = mWorld.GetActor<TActorDragPath>( Drag.mActorDragPath );
		if ( pDragPath )
			Path = pDragPath->mWorldPath;

		//	note: length is in world space! probbaly want to change to screen spatce
		if ( Path.GetLength() < 3.f )
			return;

		TGamePacket_FireMissile Packet;
		Packet.mFiringPath = Path;
		Packet.mPlayerRef = Drag.mPlayer;
		mGamePackets.PushPacket( Packet );
	}
	
	
	//	clean up drag graphics 
	if ( Drag.mActorDrag  )
	{
		mWorld.DestroyActor( *Drag.mActorDrag );
		Drag.mActorDrag = NULL;
	}
	if ( Drag.mActorDragPath  )
	{
		mWorld.DestroyActor( *Drag.mActorDragPath );
		Drag.mActorDragPath = NULL;
	}
}

void TGame::UpdateDrag(TPlayerDrag& Drag)
{
	auto* pSentry = mWorld.GetActor<TActorSentry>( Drag.mSentry );
	if ( !pSentry )
	{
		assert( pSentry );
		return;
	}
	auto& Sentry = *pSentry;

	if ( Drag.mActorDrag )
	{
		//	update actor graphics/sound etc
		ofLine2 WorldLine = Drag.GetWorldDragLine( mWorld, *this );
		Drag.mActorDrag->SetLine( WorldLine );
	}

	if ( Drag.mActorDragPath )
	{
		vec2f WorldPos = Drag.GetWorldDragTo( *this, Sentry.GetZ() );
		Drag.mActorDragPath->UpdateTail( WorldPos );
	}

	if ( Sentry.GetType() == TActors::SentryRotation )
	{
		//	rotate the death star
		TPlayer* pPlayer = GetPlayer( Sentry.GetRef() );
		if ( pPlayer && pPlayer->mDeathStar )
		{
			//	work out rotation from where the finger is to the center of the deathstar
			vec2f DeathStarCenterSceen = WorldToScreen( pPlayer->mDeathStar->GetWorldPosition3() );
			vec2f SentryCenterSceen = WorldToScreen( Sentry.GetWorldPosition3() );
			vec2f NormalToDrag = Drag.GetScreenDragTo() - DeathStarCenterSceen;
			float AngleToDrag = ofAngleFromNormal(NormalToDrag);
			float AngleToSentry = ofAngleFromNormal( Sentry.GetLocalPosition2() );
			float Rotation = AngleToDrag - AngleToSentry;
			pPlayer->mDeathStar->SetWorldRotation( Rotation );
		}
	}

	
}

void TGame::UpdateDrags()
{
	//	update the drag gestures
	for ( int i=mPendingDrags.GetSize()-1;	i>=0;	i-- )
	{
		auto& Drag = mPendingDrags[i];

		//	handle finsihed ones
		if ( Drag.mState == TPlayerDragState::Finished )
		{
			OnDragEnded( Drag );
			mPendingDrags.RemoveBlock( i,1 );
			continue;
		}

		if ( Drag.mState == TPlayerDragState::Init )
			OnDragStarted( Drag );

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
	#define case_OnPacket(PACKETTYPE)	case PACKETTYPE::TYPE:	OnPacket( static_cast<PACKETTYPE&>( Packet ) );	return true;

	switch ( Packet.GetType() )
	{
		case_OnPacket( TGamePacket_FireRocket );
		case_OnPacket( TGamePacket_FireMissile );
		case_OnPacket( TGamePacket_CollisionProjectileAndPlayer );
		case_OnPacket( TGamePacket_CollisionProjectileAndSentry );
		case_OnPacket( TGamePacket_CollisionProjectileAndAsteroidChunk );
	}

	#undef case_OnPacket

	return false;
}

void TGame::OnPacket(TGamePacket_FireRocket& Packet)
{
	//	create rocket actor
	TActorMeta Meta;
	Meta.mOwnerPlayer = Packet.mPlayerRef;
	mWorld.CreateActor<TActorRocket>( Packet.mFiringLine, Meta );
}

void TGame::OnPacket(TGamePacket_FireMissile& Packet)
{
	//	create rocket actor
	TActorMeta Meta;
	Meta.mOwnerPlayer = Packet.mPlayerRef;
	mWorld.CreateActor<TActorMissile>( Packet.mFiringPath, Meta );
}


void TGame::OnPacket(TGamePacket_CollisionProjectileAndSentry& Packet)
{
	//	actor A is the rocket
	mWorld.CreateActor<TActorExplosion>( Packet.mIntersection.mCollisionPointB );
	mWorld.DestroyActor( Packet.mActorProjectile );

	//	destroy the sentry we just hit
	mWorld.DestroyActor( Packet.mActorSentry );
}

void TGame::OnPacket(TGamePacket_CollisionProjectileAndPlayer& Packet)
{
	//	actor A is the rocket
	mWorld.CreateActor<TActorExplosion>( Packet.mIntersection.mCollisionPointB );
	mWorld.DestroyActor( Packet.mActorProjectile );

	//	find the player
	TPlayer* pPlayer = GetPlayer( Packet.mActorDeathStar );
	if ( pPlayer )
	{
		pPlayer->mHealth -= 10;
	}
}

void TGame::OnPacket(TGamePacket_CollisionProjectileAndAsteroidChunk& Packet)
{
	//	create explossion
	mWorld.CreateActor<TActorExplosion>( Packet.mIntersection.mCollisionPointB );

	//	destroy rocket
	mWorld.DestroyActor( Packet.mActorProjectile );

	//	impact asteroid
	//	gr: the impact effect should be dictated in the packet!
	auto* pChunk = mWorld.GetActor<TActorAsteroidChunk>( Packet.mActorAsteroidChunk );
	if ( pChunk )
	{
		pChunk->OnImpact( Packet.mChunkImpactMeta );
	}
}

TPlayer* TGame::GetPlayer(TActorRef ActorRef)
{
	//	grab the actor...
	TActor* pActor = mWorld.GetActor( ActorRef );

	for ( int p=0;	p<mPlayers.GetSize();	p++ )
	{
		auto& Player = mPlayers[p];

		//	match an actor a player
		if ( Player.mDeathStar == pActor )
			return &Player;

		if ( Player.mDeathStar->HasChild( ActorRef ) )
			return &Player;
	}

	return NULL;
}




ofLine2 TPlayerDrag::GetWorldDragLine(TWorld& World,TGame& Game) const
{
	//	get actor
	auto* pSentry = World.GetActor( mSentry );
	if ( !pSentry )
		return ofLine2();

	float z = pSentry->GetZ();
	vec2f WorldDragTo = this->GetWorldDragTo( Game, z );
	ofShapeCircle2 SentryShape = pSentry->GetWorldCollisionShape().mCircle;
	return ofLine2( SentryShape.mPosition, WorldDragTo );
}


//	current player (though may not have any control)
TRef TGame::GetCurrentPlayer() const
{
	switch ( mGameState )
	{
	case TGameState::PlayerOneTurn:
	case TGameState::PlayerOneTurnEnd:
		return mPlayers[0].mRef;

	case TGameState::PlayerTwoTurn:
	case TGameState::PlayerTwoTurnEnd:
		return mPlayers[1].mRef;
	}

	assert(false);
	return TRef();
}

//	current player with control (invalid if cannot control)
TRef TGame::GetCurrentControlPlayer() const
{
	switch ( mGameState )
	{
	case TGameState::PlayerOneTurn:
		return mPlayers[0].mRef;

	case TGameState::PlayerTwoTurn:
		return mPlayers[1].mRef;

	default:
		assert(false);
	case TGameState::PlayerOneTurnEnd:
	case TGameState::PlayerTwoTurnEnd:
		return TRef();
	}
}


