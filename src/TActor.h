#pragma once

#include "Main.h"
#include "TRender.h"
#include "TComponent.h"

class TWorld;	//	cyclic dependancy!
class TActor;
class TActorSentry;

#define WORLD_WIDTH	1000

#define STARS_Z		1
#define DEATHSTAR_Z	2
#define LASERBEAM_Z	3
#define SENTRY_Z	4
#define ROCKET_Z	5
#define EXPLOSION_Z	6
#define GUI_Z		7
#define DRAG_Z		GUI_Z

#define	ROCKET_SIZE		20.f
#define DEATHSTAR_SIZE	110.f
#define SENTRY_RADIUS	10.f
#define SENTRY_COUNT	10
#define ROCKET_MIN_SPEED		400.f	//	unit/sec
#define ROCKET_MAX_SPEED		400.f	//	unit/sec
#define EXPLOSION_GROW_RATE		200.f	//	unit/sec
#define EXPLOSION_MAX_SIZE		40.f
#define EXPLOSION_INITIAL_SIZE	2.f
#define LASERBEAM_RETRACT_SPEED	2000.f
#define LASERBEAM_EXTEND_SPEED	1000.f
#define LASERBEAM_WIDTH			20.f
#define LASERBEAM_MIN_LENGTH	30.f
#define DRAG_WIDTH				10.f



namespace TActors
{
	enum Type
	{
		Root,
		DeathStar,
		Stars,
		Rocket,
		Drag,
		Explosion,
		Sentry_Rocket,
		Sentry_LaserBeam,
		Sentry_Rotation,
		LaserBeam,

	};
};

//---------------------------------
//	init stuff
//---------------------------------
class TActorMeta : public TTransform
{
public:
	TTransform&			GetTransform()			{	return *this;	}
	const TTransform&	GetTransform() const	{	return *this;	}

public:
	ofColour			mColour;
	TRef				mOwnerPlayer;
};




class TActor
{
public:
	TActor()
	{
	}
	virtual ~TActor();

	virtual TActors::Type		GetType() const=0;
	TActorRef					GetRef() const					{	return TActorRef( *this );	}
	TActorRef					GetParent()						{	return mParent;	}
	virtual Array<TActorRef>	GetChildren()					{	Array<TActorRef> NoChildren;	return NoChildren;	}
	void						SetParent(TActorRef Parent);
	bool						HasChild(TActorRef ChildRef);	//	look down the heirachy for this actor
	virtual void				OnChildDestroyed(TActorRef ChildRef)	{}

	virtual void				Render(float TimeStep,const TRenderSettings& RenderSettings);
	virtual bool				Update(float TimeStep,TWorld& World);		//	return false to die
	
	virtual ofColour		GetColour() const					{	return ofColour(255,255,0);	}
	float					GetZ() const;
	vec3f					GetWorldPosition3() const 			{	vec2f Pos2 = GetWorldPosition2();	return vec3f( Pos2.x, Pos2.y, GetZ() );	}
	vec2f					GetWorldPosition2() const;
	void					SetWorldPosition(const vec2f& Pos);
	vec3f					GetLocalPosition3() const 			{	vec2f Pos2 = GetLocalPosition2();	return vec3f( Pos2.x, Pos2.y, GetZ() );	}
	vec2f					GetLocalPosition2() const;
	void					SetLocalPosition(const vec2f& Pos);
	TCollisionShape			GetWorldCollisionShape();
	TCollisionShape			GetLocalCollisionShape();
	TTransform				GetParentWorldTransform() const;
	TTransform				GetWorldTransform() const;
	void					SetWorldRotation(float AngleDeg);

	template<class TCOM>
	TCOM*					GetComponent()					{	return GetComponentContainer<TCOM>().Find( GetRef() );	}
	template<class TCOM>
	const TCOM*				GetComponent() const			{	return GetComponentContainer<TCOM>().Find( GetRef() );	}
	template<class TCOM>
	TCOM&					AddComponent()					{	return GetComponentContainer<TCOM>().Add( TComMeta( GetRef() ) );	}
	template<class TCOM,class TMETA>
	TCOM&					AddComponent(const TMETA& Meta)	{	return GetComponentContainer<TCOM>().Add( TComMeta( GetRef() ), Meta );	}

	inline bool				operator==(const TActor& Actor) const		{	return this == &Actor;	}
	inline bool				operator==(const TActors::Type& Type) const	{	return GetType() == Type;	}

private:
	TActorRef				mParent;
};




template<TActors::Type ACTORTYPE>
class TActorDerivitive : public TActor
{
public:
	static const TActors::Type TYPE = ACTORTYPE;

public:
	virtual TActors::Type	GetType() const	{	return ACTORTYPE;	}
};

class TActorDeathStar : public TActorDerivitive<TActors::DeathStar>
{
public:
	TActorDeathStar(TWorld& World,const TActorMeta& Meta);

	virtual bool				Update(float TimeStep,TWorld& World);		//	return false to die
	virtual ofColour			GetColour() const			{	return mColour;	}
	virtual Array<TActorRef>	GetChildren();
	void						GetSentrys(TWorld& World,Array<TActorSentry*>& Sentrys);
	virtual void				OnChildDestroyed(TActorRef ChildRef);

public:
	ofColour			mColour;
	Array<TActorRef>	mSentrys;
	
	float				mOffset;
	vec2f				mInitialPos;
};


class TActorStars : public TActorDerivitive<TActors::Stars>
{
public:
	TActorStars();

	virtual void		Render(float TimeStep,const TRenderSettings& RenderSettings);

	virtual ofColour	GetColour() const			{	return ofColour( 255,255,255 );	}

public:
	Array<vec3f>		mStars;
};


class TActorDrag : public TActorDerivitive<TActors::Drag>
{
public:
	virtual void		Render(float TimeStep,const TRenderSettings& RenderSettings);

	virtual ofColour	GetColour() const			{	return ofColour( 230,10,10 );	}
	void				SetLine(const ofLine2& Line);

public:
	ofLine2		mLine;
};



class TActorRocket : public TActorDerivitive<TActors::Rocket>
{
public:
	TActorRocket(const ofLine2& FiringLine,const TActorMeta& ActorMeta);

	virtual bool			Update(float TimeStep,TWorld& World);
	
public:
	TRef				mPlayerRef;		//	player which fired this rocket so we can ignore collisions from the source player (ie. at firing time)
	vec2f				mVelocity;
};



class TActorExplosion : public TActorDerivitive<TActors::Explosion>
{
public:
	TActorExplosion(const vec2f& Position);

	virtual bool			Update(float TimeStep,TWorld& World);
};


namespace TActorSentryState
{
	enum Type
	{
		Inactive,
		Active,
	};
};

class TActorSentry : public TActor
{
public:
	TActorSentry(const TActorMeta& ActorMeta);

	virtual ofColour		GetColour() const			{	return IsActive() ? ofColour(255,255,255) : mColour;	}

	TActorSentryState::Type	GetState() const			{	return mState;	}
	virtual void			SetState(TActorSentryState::Type State)=0;
	bool					IsActive() const			{	return GetState() == TActorSentryState::Active;	}
	bool					IsInactive() const			{	return GetState() == TActorSentryState::Inactive;	}

public:
	TActorSentryState::Type	mState;
	ofColour				mColour;
};


template<TActors::Type ACTORTYPE>
class TActorSentryBase : public TActorSentry
{
public:
	static const TActors::Type TYPE = ACTORTYPE;

public:
	TActorSentryBase(const TActorMeta& ActorMeta) :
		TActorSentry	( ActorMeta )
	{
	}

	virtual TActors::Type	GetType() const	{	return ACTORTYPE;	}
};

class TActorSentryRocket : public TActorSentryBase<TActors::Sentry_Rocket>
{
public:
	TActorSentryRocket(const TActorMeta& ActorMeta,TWorld& World) :
		TActorSentryBase	( ActorMeta )
	{
	}

	virtual void			SetState(TActorSentryState::Type State)		{	mState = State;	}
};


class TActorSentryRotation : public TActorSentryBase<TActors::Sentry_Rotation>
{
public:
	TActorSentryRotation(const TActorMeta& ActorMeta,TWorld& World) :
		TActorSentryBase	( ActorMeta )
	{
	}

	virtual void			SetState(TActorSentryState::Type State)		{	mState = State;	}
};

class TActorSentryLaserBeam : public TActorSentryBase<TActors::Sentry_LaserBeam>
{
public:
	TActorSentryLaserBeam(const TActorMeta& ActorMeta,TWorld& World);

	virtual bool				Update(float TimeStep,TWorld& World);
	virtual void				OnChildDestroyed(TActorRef ChildRef);
	virtual void				SetState(TActorSentryState::Type State);
	virtual Array<TActorRef>	GetChildren();

public:
	TActorRef				mLaserBeam;
};

class TActorLaserBeam : public TActorDerivitive<TActors::LaserBeam>
{
public:
	TActorLaserBeam(const TActorMeta& ActorMeta);

	virtual ofColour		GetColour() const			{	return ofColour( 255,90,220 );	}
	virtual void			Render(float TimeStep,const TRenderSettings& RenderSettings);
	virtual bool			Update(float TimeStep,TWorld& World);

	ofLine2					GetWorldBeamLine() const;

public:
	bool					mActive;
	float					mLength;
};

