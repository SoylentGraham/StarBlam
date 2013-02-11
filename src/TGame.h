#pragma once

#include "Main.h"
#include "TWorld.h"
#include "TGamePackets.h"


class TGameCamera
{
public:
	TGameCamera() :
		mZoom	( 10.2f )
	{
	}

public:
	vec2f		mScroll;
	float		mZoom;
};


class TPlayerMeta
{
public:
	TPlayerMeta(const String& Name,const ofColour& Colour) :
		mName	( Name ),
		mColour	( Colour )
	{
	}

public:
	ofColour	mColour;
	String		mName;
};


class TPlayer
{
public:
	TPlayer() :
		mMeta		( "", ofColour(0,0,0) ),
		mDeathStar	( NULL )
	{
	}
		
	TPlayer(const TPlayerMeta& Meta) :
		mMeta		( Meta ),
		mDeathStar	( NULL )
	{
	}

public:
	TActorDeathStar*	mDeathStar;
	TPlayerMeta			mMeta;
};


class TPlayerDrag
{
public:
	TPlayerDrag() :
		mFinished	( false ),
		mActor		( NULL )
	{
	}

public:
	bool			mFinished;	//	if true the drag hasn't been relesaed yet
	ofShapeLine2	mLine;
	TActorDrag*		mActor;
};



//	disposable game container with logic
class TGame
{
public:
	TGame(const TPlayerMeta& Player1,const TPlayerMeta& Player2);

	bool		Init();

	void		Update(float TimeStep);
	void		UpdateInput(SoyInput& Input);

	void		Render(float TimeStep);

protected:
	void		UpdateDrags();
	void		OnDragStarted(TPlayerDrag& Drag);
	void		OnDragEnded(TPlayerDrag& Drag);
	void		UpdateDrag(TPlayerDrag& Drag);
	
	void		UpdateGamePackets();
	bool		OnPacket(TGamePacket& Packet);
	void		OnPacket_FireRocket(TGamePacket_FireRocket& Packet);

	vec2f		ScreenToWorld(const vec2f& Screen2,float Z);

public:
	TGamePacketContainer	mGamePackets;
	ofRectangle				mOrthoViewport;
	TGameCamera				mGameCamera;
	ofCamera				mCamera;
	TWorld					mWorld;
	Array<TPlayer>			mPlayers;
	Array<TPlayerDrag>		mPendingDrags;
};


