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
#define ASTEROID_Z	4
#define SENTRY_Z	5
#define ROCKET_Z	6
#define EXPLOSION_Z	7
#define GUI_Z		8
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
#define LASERBEAM_EXTEND_SPEED	20000.f
#define LASERBEAM_WIDTH			20.f
#define LASERBEAM_MIN_LENGTH	30.f
#define LASERBEAM_MAX_LENGTH	20000.f
#define DRAG_WIDTH				10.f
#define GRAVITY_FORCE_FACTOR	15.f


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
		Asteroid,
		AsteroidChunk,
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
	TActor(const TActorMeta& Meta);
	virtual ~TActor();

	virtual TActors::Type		GetType() const=0;
	TActorRef					GetRef() const					{	return TActorRef( *this );	}
	TActorRef					GetParent() const				{	return mParent;	}
	virtual Array<TActorRef>	GetChildren() const				{	Array<TActorRef> NoChildren;	return NoChildren;	}
	void						SetParent(TActorRef Parent);
	bool						HasChild(TActorRef ChildRef) const;	//	look down the heirachy for this actor
	virtual void				OnChildDestroyed(TActorRef ChildRef,TWorld& World)	{}
	virtual void				OnChildAdded(TActor& Child,TWorld& World)		{}

	virtual void				Render(float TimeStep,const TRenderSettings& RenderSettings);
	virtual bool				Update(float TimeStep,TWorld& World);		//	return false to die
	
	virtual ofColour		GetColour() const					{	return ofColour(255,255,0);	}
	TRef					GetOwnerPlayer() const;

	float					GetZ() const;
	vec3f					GetWorldPosition3() const 			{	vec2f Pos2 = GetWorldPosition2();	return vec3f( Pos2.x, Pos2.y, GetZ() );	}
	vec2f					GetWorldPosition2() const;
	void					SetWorldPosition(const vec2f& Pos);
	vec3f					GetLocalPosition3() const 			{	vec2f Pos2 = GetLocalPosition2();	return vec3f( Pos2.x, Pos2.y, GetZ() );	}
	vec2f					GetLocalPosition2() const;
	void					SetLocalPosition(const vec2f& Pos);
	TCollisionShape			GetWorldCollisionShape() const;
	TCollisionShape			GetLocalCollisionShape() const;
	TTransform				GetParentWorldTransform() const;
	TTransform				GetWorldTransform() const;
	void					SetWorldRotation(float AngleDeg);

	ofShapeCircle2			GetTreeLocalCollisionShape(TWorld& World) const;		//	accumulate the collision shape of all children in the tree, localised to this

	template<class TCOM>
	TCOM*					GetComponent()					{	return TComs::GetContainer<TCOM>().Find( GetRef() );	}
	template<class TCOM>
	const TCOM*				GetComponent() const			{	return TComs::GetContainer<TCOM>().Find( GetRef() );	}
	template<class TCOM>
	TCOM&					AddComponent()					{	return TComs::GetContainer<TCOM>().Add( TComMeta( GetRef() ) );	}
	template<class TCOM,class TMETA>
	TCOM&					AddComponent(const TMETA& Meta)	{	return TComs::GetContainer<TCOM>().Add( TComMeta( GetRef() ), Meta );	}

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
	TActorDerivitive()	{}
	TActorDerivitive(const TActorMeta& Meta) : 
		TActor	( Meta )
	{
	}
	virtual TActors::Type	GetType() const	{	return ACTORTYPE;	}
};

class TActorDeathStar : public TActorDerivitive<TActors::DeathStar>
{
public:
	TActorDeathStar(const TActorMeta& Meta,TWorld& World);

	virtual bool				Update(float TimeStep,TWorld& World);		//	return false to die
	virtual ofColour			GetColour() const			{	return mColour;	}
	virtual Array<TActorRef>	GetChildren() const;
	void						GetSentrys(TWorld& World,Array<TActorSentry*>& Sentrys);
	virtual void				OnChildDestroyed(TActorRef ChildRef,TWorld& World);

public:
	ofColour			mColour;
	Array<TActorRef>	mSentrys;
	
	float				mOffset;
	vec2f				mInitialPos;
};


class TActorStars : public TActorDerivitive<TActors::Stars>
{
public:
	TActorStars(TWorld& World);

	virtual void		Render(float TimeStep,const TRenderSettings& RenderSettings);

	virtual ofColour	GetColour() const			{	return ofColour( 255,255,255 );	}

public:
	Array<vec3f>		mStars;
};


class TActorDrag : public TActorDerivitive<TActors::Drag>
{
public:
	TActorDrag(TWorld& World)	{}

	virtual void		Render(float TimeStep,const TRenderSettings& RenderSettings);

	virtual ofColour	GetColour() const			{	return ofColour( 230,10,10 );	}
	void				SetLine(const ofLine2& Line);

public:
	ofLine2		mLine;
};



class TActorRocket : public TActorDerivitive<TActors::Rocket>
{
public:
	TActorRocket(const ofLine2& FiringLine,const TActorMeta& ActorMeta,TWorld& World);

	virtual bool			Update(float TimeStep,TWorld& World);
	
public:
	vec2f				mVelocity;
};



class TActorExplosion : public TActorDerivitive<TActors::Explosion>
{
public:
	TActorExplosion(const vec2f& Position,TWorld& World);

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
	virtual void				OnChildDestroyed(TActorRef ChildRef,TWorld& World);
	virtual void				SetState(TActorSentryState::Type State);
	virtual Array<TActorRef>	GetChildren() const;

public:
	TActorRef				mLaserBeam;
};

class TActorLaserBeam : public TActorDerivitive<TActors::LaserBeam>
{
public:
	TActorLaserBeam(const TActorMeta& ActorMeta,TWorld& World);

	virtual ofColour		GetColour() const			{	return ofColour( 255,90,220 );	}
	virtual void			Render(float TimeStep,const TRenderSettings& RenderSettings);
	virtual bool			Update(float TimeStep,TWorld& World);

	ofLine2					GetWorldBeamLine() const;

public:
	bool					mActive;
	float					mLength;
};


//------------------------------------
//	an asteroid is made of chunks that break off as they're destroyed. 
//	if one is totally broken up (but not destroyed) they become individual asteroids
//	a chunk alone has no gravity
class TActorAsteroid : public TActorDerivitive<TActors::Asteroid>
{
public:
	TActorAsteroid(const TActorMeta& ActorMeta,float Radius,TWorld& World);

	virtual bool				Update(float TimeStep,TWorld& World);
	virtual Array<TActorRef>	GetChildren() const									{	return mChunks;	}
	virtual void				OnChildDestroyed(TActorRef ChildRef,TWorld& World)	{	mChunks.Remove( ChildRef );	RecalcGravity( World );	}
	virtual void				OnChildAdded(TActor& Child,TWorld& World)			{	RecalcGravity( World );	}
	void						RecalcGravity(TWorld& World);

public:
	Array<TActorRef>			mChunks;
};


class TAsteroidChunkImpactMeta
{
public:
	TAsteroidChunkImpactMeta() :
		mChunkHealthReduction	( 1.f ),
		mSplitAsteroid			( false ),
		mChunkForce				( 0,0 )
	{
	}
public:
	float			mChunkHealthReduction;	//	reduce chunk by this much
	bool			mSplitAsteroid;			//	should we break up the parent asteroid?
	vec2f			mChunkForce;			//	impact the asteroid by this amount of force
};

class TActorAsteroidChunk : public TActorDerivitive<TActors::AsteroidChunk>
{
public:
	TActorAsteroidChunk(const ofShapePolygon2& Polygon,TWorld& World);

	virtual bool	Update(float TimeStep,TWorld& World);
	void			OnImpact(const TAsteroidChunkImpactMeta& Impact);

public:
	float			mHealth;	//	0..1
};

