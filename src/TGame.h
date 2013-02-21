#pragma once

#include "Main.h"
#include "TWorld.h"
#include "TStarWorld.h"
#include "TGamePackets.h"
#include "TActor.h"

#define ENABLE_STARS		true
#define FINGER_SCREEN_SIZE	10.f	//	may vary for different devices at different DPI...
#define ASTEROID_COUNT		15

class TGame;


namespace TGameState
{
	enum Type		
	{
		PlayerOneTurn,
		PlayerOneTurnEnd,	//	waiting for everything to finish
		PlayerTwoTurn,
		PlayerTwoTurnEnd,
	};
};


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
	int						mHealth;
};

namespace TPlayerDragState
{
	enum Type
	{
		Init,
		Active,
		Finished,
	};
};

class TPlayerDrag
{
public:
	TPlayerDrag() :
		mState		( TPlayerDragState::Init ),
		mActor		( NULL )
	{
	}

	bool			IsFinished() const				{	return mState == TPlayerDragState::Finished;	}
	void			SetFinished()					{	mState = TPlayerDragState::Finished;	}
	ofLine2			GetWorldDragLine(TWorld& World,TGame& Game) const;
	vec2f			GetWorldDragTo(TGame& Game,float z) const;
	vec2f			GetScreenDragTo() const					{	return mScreenDragTo;	}
	void			SetScreenDragTo(const vec2f& ScreenPos)	{	mScreenDragTo = ScreenPos;	}

public:
	TPlayerDragState::Type	mState;			//	if true the drag hasn't been relesaed yet
	TActorRef				mSentry;		//	drag from this sentry
	TActorDrag*				mActor;			//	visualisation of drag
	TRef					mPlayer;		//	player which did the drag

private:
	vec3f					mScreenDragTo;	//	where the player is dragging to
};


//----------------------------------------------
//	disposable game container with logic
//	controls the real-reaction with game happenings
//----------------------------------------------
class TGame
{
public:
	TGame(const TPlayerMeta& Player1,const TPlayerMeta& Player2);

	bool			Init();

	void			Update(float TimeStep);
	void			UpdateInput(SoyInput& Input);

	void			Render(float TimeStep);
	vec2f			ScreenToWorld(const vec2f& Screen2,float Z);
	vec2f			WorldToScreen(const vec3f& World3);
	ofShapeCircle2	WorldToScreen(const ofShapeCircle2& Shape,float Z);

	//	world sim->game
	template<class TPACKET>
	void			PushPacket(const TPACKET& Packet)	{	mGamePackets.PushPacket( Packet );	}
	
	//	utils
	TPlayer*		GetPlayer(TActorRef ActorRef);		//	find the owner of this actor
	TPlayer*		GetPlayer(TRef PlayerRef)			{	return mPlayers.Find( PlayerRef );	}
	TRef			GetCurrentPlayer() const;			//	current player (though may not have any control)
	TRef			GetCurrentControlPlayer() const;	//	current player with control (invalid if cannot control)

protected:
	void			RenderWorld(float TimeStep);
	void			RenderHud(float TimeStep);

	//	local
	void			UpdateDrags();
	bool			TryNewDrag(const SoyGesture& Gesture);
	void			OnDragStarted(TPlayerDrag& Drag);
	void			OnDragEnded(TPlayerDrag& Drag);
	void			UpdateDrag(TPlayerDrag& Drag);
	

	//	real
	void			UpdateGamePackets();
	bool			OnPacket(TGamePacket& Packet);
	void			OnPacket(TGamePacket_FireRocket& Packet);
	void			OnPacket(TGamePacket_CollisionRocketPlayer& Packet);
	void			OnPacket(TGamePacket_CollisionRocketAndSentry& Packet);
	void			OnPacket(TGamePacket_CollisionRocketAndAsteroidChunk& Packet);



public:
	TGamePacketContainer	mGamePackets;
	ofRectangle				mOrthoViewport;
	TGameCamera				mGameCamera;
	ofCamera				mCamera;
	TStarWorld				mWorld;
	Array<TPlayer>			mPlayers;
	TGameState::Type		mGameState;
	Array<TPlayerDrag>		mPendingDrags;
};

