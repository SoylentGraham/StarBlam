#pragma once

#include "Main.h"
#include "TWorld.h"
#include "TGamePackets.h"


#define SENTRY_COUNT	10
#define SENTRY_RADIUS	10.f

#define FINGER_SCREEN_SIZE	10.f	//	may vary for different devices at different DPI...


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

	inline bool	operator==(const TRef& Ref) const	{	return mRef == Ref;	}

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
	TActorDeathStar*		mDeathStar;
	Array<TActorSentry*>	mSentrys;
	int						mHealth;
};


class TPlayerDrag
{
public:
	TPlayerDrag() :
		mFinished	( false ),
		mActor		( NULL )
	{
	}

	ofLine2			GetLine(TWorld& World) const;

public:
	bool			mFinished;	//	if true the drag hasn't been relesaed yet
	TActorRef		mSentry;	//	drag from this sentry
	vec2f			mDragTo;	//	where the player is dragging to
	TActorDrag*		mActor;		//	visualisation of drag
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
	bool		TryNewDrag(const SoyGesture& Gesture);
	void		OnDragStarted(TPlayerDrag& Drag);
	void		OnDragEnded(TPlayerDrag& Drag);
	void		UpdateDrag(TPlayerDrag& Drag);
	
	void		UpdateCollisions();
	void		OnCollision(const TCollision& Collision,TActor& ActorA,TActor& ActorB);
	void		OnCollision(const TCollision& Collision,TActorRocket& ActorA,TActorDeathStar& ActorB);
	void		OnCollision(const TCollision& Collision,TActorRocket& ActorA,TActorSentry& ActorB);
	template<class ACTORA,class ACTORB>
	bool		HandleCollision(const TCollision& Collision,TActor& ActorA,TActor& ActorB);

	//	real
	void		UpdateGamePackets();
	bool		OnPacket(TGamePacket& Packet);
	void		OnPacket_FireRocket(TGamePacket_FireRocket& Packet);
	void		OnPacket_Collision(TGamePacket_CollisionRocketPlayer& Packet);
	void		OnPacket_Collision(TGamePacket_CollisionRocketSentry& Packet);

	//	utils
	TPlayer*	GetPlayer(TActorRef ActorRef);		//	find the owner of this actor
	TPlayer*	GetPlayer(TRef PlayerRef)			{	return mPlayers.Find( PlayerRef );	}
	TRef		GetCurrentPlayer() const			{	return mCurrentPlayer;	}

	vec2f			ScreenToWorld(const vec2f& Screen2,float Z);
	vec2f			WorldToScreen(const vec3f& World3);
	ofShapeCircle2	WorldToScreen(const ofShapeCircle2& Shape,float Z);

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

