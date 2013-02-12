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
	TPlayerMeta(const TString& Name,const ofColour& Colour);

public:
	ofColour	mColour;
	TString		mName;
	TRef		mRef;
};


class TPlayer : public TPlayerMeta
{
public:
	TPlayer() :
		TPlayerMeta	( "", ofColour(0,0,0) ),
		mDeathStar	( NULL ),
		mHealth		( 100 )
	{
	}
		
	TPlayer(const TPlayerMeta& Meta) :
		TPlayerMeta	( Meta ),
		mDeathStar	( NULL ),
		mHealth		( 100 )
	{
	}

	TPlayerMeta&		GetMeta()		{	return *this;	}

public:
	TActorDeathStar*	mDeathStar;
	int					mHealth;
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
	TRef			mPlayer;	//	player which did the drag
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
	void		RenderWorld(float TimeStep);
	void		RenderHud(float TimeStep);

	//	local
	void		UpdateDrags();
	void		OnDragStarted(TPlayerDrag& Drag);
	void		OnDragEnded(TPlayerDrag& Drag);
	void		UpdateDrag(TPlayerDrag& Drag);
	
	void		UpdateCollisions();
	void		OnCollision(const TCollision& Collision,TActor& ActorA,TActor& ActorB);
	void		OnCollision(const TCollision& Collision,TActorRocket& ActorA,TActorDeathStar& ActorB);
	template<class ACTORA,class ACTORB>
	bool		HandleCollision(const TCollision& Collision,TActor& ActorA,TActor& ActorB);

	//	real
	void		UpdateGamePackets();
	bool		OnPacket(TGamePacket& Packet);
	void		OnPacket_FireRocket(TGamePacket_FireRocket& Packet);
	void		OnPacket_Collision(TGamePacket_CollisionRocketPlayer& Packet);

	//	utils
	TPlayer*	GetPlayer(TActorRef ActorRef);		//	find the owner of this actor
	TRef		GetCurrentPlayer() const			{	return mCurrentPlayer;	}

	vec2f		ScreenToWorld(const vec2f& Screen2,float Z);
	vec2f		WorldToScreen(const vec3f& World3);

public:
	TGamePacketContainer	mGamePackets;
	ofRectangle				mOrthoViewport;
	TGameCamera				mGameCamera;
	ofCamera				mCamera;
	TWorld					mWorld;
	Array<TPlayer>			mPlayers;
	TRef					mCurrentPlayer;		//	who's turn
	Array<TPlayerDrag>		mPendingDrags;
};


template<class ACTORA,class ACTORB>
bool TGame::HandleCollision(const TCollision& Collision,TActor& ActorA,TActor& ActorB)
{
	if ( ActorA == ACTORA::TYPE && ActorB == ACTORB::TYPE )
	{
		OnCollision( Collision, static_cast<ACTORA&>( ActorA ), static_cast<ACTORB&>( ActorB ) );
		return true;
	}
	if ( ActorA == ACTORB::TYPE && ActorA == ACTORB::TYPE )
	{
		OnCollision( Collision, static_cast<ACTORB&>( ActorB ), static_cast<ACTORA&>( ActorA ) );
		return true;
	}
	return false;
}

