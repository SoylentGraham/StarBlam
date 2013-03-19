#pragma once

#include "Main.h"
#include "TGameMeta.h"
#include "TWorld.h"
#include "TStarWorld.h"
#include "TGamePackets.h"
#include "TActor.h"

#define ENABLE_STARS		true
#define FINGER_SCREEN_SIZE	10.f	//	may vary for different devices at different DPI...
#define ASTEROID_COUNT		15

class TGame;


namespace TGameStates
{
	enum Type		
	{
		Invalid,
		Init,

		PlayerTurn,
		PlayerTurnEnd,	//	waiting for everything to finish
		
		ChangingState,		//	waiting for a packet to change turn

		Exit_Error,
	};
	
	BufferString<100>	ToString(TGameStates::Type Enum);
	void				GetArray(ArrayBridge<TGameStates::Type>& Array);
};
SOY_DECLARE_ENUM( TGameStates );


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


class TPlayerDrag
{
public:
	TPlayerDrag() :
		mActive			( true ),
		mActorDrag		( NULL ),
		mActorDragPath	( NULL )
	{
	}

	bool			IsFinished() const				{	return !mActive;	}
	void			SetFinished()					{	mActive = false;	}
	ofLine2			GetWorldDragLine(TWorld& World,TGame& Game) const;
	vec2f			GetWorldDragTo(TGame& Game,float z) const;
	vec2f			GetScreenDragTo() const					{	return mScreenDragTo;	}
	bool			SetScreenDragTo(const vec2f& ScreenPos);	//	returns if changed

	inline bool		operator==(const TPlayerDrag& That) const	{	return this == &That;	}

public:
	bool					mActive;		//	if true the drag hasn't been relesaed yet
	TActorRef				mSentry;		//	drag from this sentry
	TRef					mPlayer;		//	player which did the drag
	//	todo: merge!
	TActorDrag*				mActorDrag;			//	visualisation of drag
	TActorDragPath*			mActorDragPath;		//	visualisation of drag

private:
	vec3f					mScreenDragTo;	//	where the player is dragging to
};


class TGameTurnEnder
{
public:
	virtual ~TGameTurnEnder()	{}

	virtual bool	Update(float TimeStep,TGame& Game)=0;	//	return if still waiting
};


class TGameTurnEnder_ActorDeath : public TGameTurnEnder
{
public:
	TGameTurnEnder_ActorDeath(TActorRef Actor) :
		mActor	( Actor )
	{
	}

	virtual bool	Update(float TimeStep,TGame& Game);

public:
	TActorRef	mActor;
};


class TGameState
{
public:
	TGameState() :
		mState	( TGameStates::Invalid )
	{
	}
	TGameState(TGameStates::Type State,SoyRef Player=SoyRef()) :
		mState	( State ),
		mPlayer	( Player )
	{
	}

	bool				IsValid() const	{	return mState != TGameStates::Invalid;	}

public:
	TGameStates::Type	mState;
	SoyRef				mPlayer;
};

template<class STRING>
inline STRING& operator<<(STRING& str,const TGameState& State)
{
	str << SoyEnum::ToString(State.mState) << " (player " << State.mPlayer << ")";
	return str;
}

//----------------------------------------------
//	disposable game container with logic
//	controls the real-reaction with game happenings
//----------------------------------------------
class TGame
{
public:
	TGame(const TGameMeta& GameMeta);

	SoyRef			GetRef() const			{	return SoyRef("Game");	}	//	will be module ref

	bool			Init();
	void			Update(float TimeStep);
	void			UpdateInput(SoyInput& Input);

	void			Render(float TimeStep);
	vec2f			ScreenToWorld(const vec2f& Screen2,float Z);
	vec2f			WorldToScreen(const vec3f& World3);
	ofShapeCircle2	WorldToScreen(const ofShapeCircle2& Shape,float Z);

	//	world sim->game
	template<class TPACKET>
	void			PushPacket(const TPACKET& Packet)	{	mGamePackets.PushPacket( SoyPacketMeta( GetRef() ), Packet );	}
	
	//	utils
	TPlayer*		GetPlayer(TActorRef ActorRef);		//	find the owner of this actor
	TPlayer*		GetPlayer(TRef PlayerRef)			{	return mPlayers.Find( PlayerRef );	}
	TRef			GetCurrentPlayer() const			{	return mGameState.mPlayer;	}	//	current player (though may not have any control)
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
	void			UpdateState(float TimeStep);
	void			SetNextPlayerTurn();				//	go from player1 turn to player2
	void			OnTurnTimeout();					//	force end of a turn when time runs out
	void			ChangeGameState(const TGameState& NewGameState,TActorRef WaitForActorDeath);
	void			ChangeGameState(const TGameState& NewGameState,TActor* pWaitForActorDeath=NULL);

	//	real
	void			UpdateGamePackets();
	bool			OnPacket(const SoyPacketContainer& Packet);
	void			OnPacket(const TGamePacket_FireRocket& Packet);
	void			OnPacket(const TGamePacket_FireMissile& Packet);
	void			OnPacket(const TGamePacket_CollisionProjectileAndPlayer& Packet);
	void			OnPacket(const TGamePacket_CollisionProjectileAndSentry& Packet);
	void			OnPacket(const TGamePacket_CollisionProjectileAndAsteroidChunk& Packet);
	void			OnPacket(const TGamePacket_ChangeTurn& Packet);
	void			OnPacket(const TGamePacket_StartDrag& Packet);
	void			OnPacket(const TGamePacket_EndDrag& Packet);
	void			OnPacket(const TGamePacket_UpdateDrag& Packet);



public:
	TGameMeta				mGameMeta;
	SoyPacketManager		mGamePackets;

	ofRectangle				mOrthoViewport;
	TGameCamera				mGameCamera;
	ofCamera				mCamera;

	TStarWorld				mWorld;
	Array<TPlayer>			mPlayers;

	Array<TPlayerDrag>		mPendingDrags;

	SoyRef					mActivePlayer;
	TGameState				mGameState;
	ofPtr<TGameTurnEnder>	mTurnEnder;
	float					mTurnTime;			//	turn countdown
};

