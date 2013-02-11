#include "TGame.h"





TGame::TGame(const TPlayerMeta& Player1,const TPlayerMeta& Player2)
{
	mPlayers.PushBack( Player1 );
	mPlayers.PushBack( Player2 );
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

		Player.mDeathStar = mWorld.CreateActor<TActorDeathStar>();
		Player.mDeathStar->mMaterial.mColour = Player.mMeta.mColour;
		Player.mDeathStar->SetPosition( PlayerPositions[p] );
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

	//	process game packets (logic basically)
	UpdateGamePackets();

	//	update entities
	mWorld.Update( TimeStep );
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


	//	render game background
	ofClear( ofColour(30,30,30) );

	//	test render
	/*
	{
		TRenderSceneScope Scene("Test");
		ofFill();
		ofSetColor( ofColour(255,0,0) );
		ofCircle( -WORLD_WIDTH/2, -200, 0, 40.f );
		ofSetColor( ofColour(0,255,0) );
		ofCircle( 0, 0, 0, 40.f );
		ofSetColor( ofColour(0,0,255) );
		ofCircle( WORLD_WIDTH/2, 0, 0, 40.f );
	}
	*/


	//	render world entities
	static bool RenderCollision = true;
	TRenderSettings RenderSettings;
	RenderSettings.mMode = RenderCollision ? TRenderMode::Collision : TRenderMode::Colour;
	mWorld.Render( TimeStep, RenderSettings );

	//	render hud
	mCamera.end();
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
			pCurrentDrag = &mPendingDrags.PushBack();
			pCurrentDrag->mLine.mStart = ScreenToWorld( Gesture.mPath[0], GUI_Z );
			pCurrentDrag->mLine.mEnd = ScreenToWorld( Gesture.mPath.GetBack(), GUI_Z );
		}
		else if ( pCurrentDrag && Gesture.IsDown() )
		{
			//	update end point of current drag
			pCurrentDrag->mLine.mEnd = ScreenToWorld( Gesture.mPath.GetBack(), GUI_Z );
		}
		
	}
	Input.PopUnlock();
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
	mWorld.DestroyActor( Drag.mActor );
	Drag.mActor = NULL;

	//	launch rocket
	TGamePacket_FireRocket Packet;
	Packet.mFiringLine = Drag.mLine;
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
	Drag.mActor->SetLine( Drag.mLine );
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
	float ScreenNormalx = Screen2.x / Viewport.width;
	float ScreenNormaly = Screen2.y  / Viewport.height;
	float OrthoX = ScreenNormalx * mOrthoViewport.width;
	float OrthoY = ScreenNormaly * mOrthoViewport.height;
	OrthoX += mOrthoViewport.x;
	OrthoY += mOrthoViewport.y;
	
	return vec2f( OrthoX, OrthoY );
	/*
	vec3f Screen3( Screen2.x, Screen2.y, Z );
	//convert from screen to camera
	ofVec3f CameraXYZ;
	CameraXYZ.x = 2.0f * (Screen3.x - viewport.x) / viewport.width - 1.0f;
	CameraXYZ.y = 1.0f - 2.0f *(Screen3.y - viewport.y) / viewport.height;
	CameraXYZ.z = Screen3.z;

	//get inverse camera matrix
	ofMatrix4x4 inverseCamera;
	inverseCamera.makeInvertOf(getModelViewProjectionMatrix(viewport));

	//convert camera to world
	vec3f World3( CameraXYZ * inverseCamera );
	
	return vec2f( World3.x, World3.y );
	*/
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
	}

	return false;
}

void TGame::OnPacket_FireRocket(TGamePacket_FireRocket& Packet)
{
	//	create rocket actor
	mWorld.CreateActor<TActorRocket>( Packet.mFiringLine );
}

