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

#define	ROCKET_SIZE		10.f
#define ROCKET_MIN_SPEED		350.f	//	unit/sec
#define ROCKET_MAX_SPEED		350.f	//	unit/sec
#define MISSILE_SPEED			ROCKET_MAX_SPEED	//	unit/sec
#define DEATHSTAR_SIZE	110.f
#define SENTRY_RADIUS	10.f
#define SENTRY_COUNT	10
#define EXPLOSION_GROW_RATE		200.f	//	unit/sec
#define EXPLOSION_MAX_SIZE		40.f
#define EXPLOSION_INITIAL_SIZE	2.f
#define LASERBEAM_RETRACT_SPEED	2000.f
#define LASERBEAM_EXTEND_SPEED	20000.f
#define LASERBEAM_WIDTH			20.f
#define LASERBEAM_MIN_LENGTH	30.f
#define LASERBEAM_MAX_LENGTH	20000.f
#define DRAG_WIDTH				10.f
#define GRAVITY_FORCE_FACTOR	40.f
#define PATH_WIDTH				1.f
#define PATH_MIN_SEGMENT_LENGTH		4.f

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
		SentryRocket,
		SentryLaserBeam,
		SentryRotation,
		LaserBeam,
		Asteroid,
		AsteroidChunk,
		BasePath,
		AutoPath,			//	automatically generate path from parent's movement
		DragPath,			//	user-defined path (for missile)
		SentryMissile,
		Missile,
	};
};

//---------------------------------
//	init stuff
//---------------------------------
class TActorMeta : public TTransform2
{
public:
	TTransform2&		GetTransform()			{	return *this;	}
	const TTransform2&	GetTransform() const	{	return *this;	}

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
	void						SetParent(TActorRef Parent,TWorld& World);
	bool						HasChild(TActorRef ChildRef) const;	//	look down the heirachy for this actor
	virtual void				OnChildReleased(TActorRef ChildRef,TWorld& World)	{}
	virtual void				OnChildAdded(TActor& Child,TWorld& World)		{}
	virtual void				OnPreDestroy(TWorld& World)		{	}

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
	TTransform2				GetParentWorldTransform() const;
	TTransform2				GetWorldTransform() const;
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
	virtual void				OnChildReleased(TActorRef ChildRef,TWorld& World);

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


class TActorProjectile : public TActor
{
public:
	TActorProjectile(const TActorMeta& ActorMeta,TWorld& World);

	virtual bool				Update(float TimeStep,TWorld& World);
	virtual void				OnChildReleased(TActorRef ChildRef,TWorld& World);
	virtual Array<TActorRef>	GetChildren() const;
	virtual void				OnPreDestroy(TWorld& World);
	vec2f						GetGravityForce(TWorld& World);

public:
	TActorRef			mAutoPath;
};

template<TActors::Type ACTORTYPE>
class TActorProjectileBase : public TActorProjectile
{
public:
	static const TActors::Type TYPE = ACTORTYPE;

public:
	TActorProjectileBase(const TActorMeta& ActorMeta,TWorld& World) :
		TActorProjectile	( ActorMeta, World )
	{
	}

	virtual TActors::Type	GetType() const	{	return ACTORTYPE;	}
};


class TActorRocket : public TActorProjectileBase<TActors::Rocket>
{
public:
	TActorRocket(const ofLine2& FiringLine,const TActorMeta& ActorMeta,TWorld& World);

	virtual bool		Update(float TimeStep,TWorld& World);

public:
	vec2f				mVelocity;
};


class TActorMissile : public TActorProjectileBase<TActors::Missile>
{
public:
	TActorMissile(const ofShapePath2& FiringPath,const TActorMeta& ActorMeta,TWorld& World);

	virtual bool	Update(float TimeStep,TWorld& World);
	void			AddVelocity(const vec2f& AdditionalVelocity);

public:
	ofShapePath2	mPath;
	float			mPathDistance;	//	how far along the path we are in units
	vec2f			mVelocity;		//	we follow the path, but we might deviate because of gravity
	vec2f			mLastDelta;		//	once we get to the end of the path, we just follow the vector at the end. which is this
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

class TActorSentryRocket : public TActorSentryBase<TActors::SentryRocket>
{
public:
	TActorSentryRocket(const TActorMeta& ActorMeta,TWorld& World) :
		TActorSentryBase	( ActorMeta )
	{
	}

	virtual void			SetState(TActorSentryState::Type State)		{	mState = State;	}
};

class TActorSentryMissile : public TActorSentryBase<TActors::SentryMissile>
{
public:
	TActorSentryMissile(const TActorMeta& ActorMeta,TWorld& World) :
		TActorSentryBase	( ActorMeta )
	{
	}

	virtual void			SetState(TActorSentryState::Type State)		{	mState = State;	}
};


class TActorSentryRotation : public TActorSentryBase<TActors::SentryRotation>
{
public:
	TActorSentryRotation(const TActorMeta& ActorMeta,TWorld& World) :
		TActorSentryBase	( ActorMeta )
	{
	}

	virtual void			SetState(TActorSentryState::Type State)		{	mState = State;	}
};

class TActorSentryLaserBeam : public TActorSentryBase<TActors::SentryLaserBeam>
{
public:
	TActorSentryLaserBeam(const TActorMeta& ActorMeta,TWorld& World);

	virtual bool				Update(float TimeStep,TWorld& World);
	virtual void				OnChildReleased(TActorRef ChildRef,TWorld& World);
	virtual void				SetState(TActorSentryState::Type State);
	virtual Array<TActorRef>	GetChildren() const;

	bool						IsLaserBeamIdle(TWorld& World) const;

public:
	TActorRef				mLaserBeam;
};

class TActorLaserBeam : public TActorDerivitive<TActors::LaserBeam>
{
public:
	TActorLaserBeam(const TActorMeta& ActorMeta,TWorld& World);

	virtual ofColour		GetColour() const			{	return ofColour( 255,90,220 );	}
	virtual bool			Update(float TimeStep,TWorld& World);

	bool					HasRetracted() const;
	ofShapeCapsule2			GetWorldBeam() const;
	void					UpdateCollisionShape();

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
	virtual void				OnChildReleased(TActorRef ChildRef,TWorld& World)	{	mChunks.Remove( ChildRef );	RecalcGravity( World );	}
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


template<TActors::Type ACTORTYPE>
class TActorPathBase : public TActorDerivitive<ACTORTYPE>
{
public:
	TActorPathBase() :
		mWorldPath	( PATH_MIN_SEGMENT_LENGTH )
	{
	}

	void				UpdateTail(const vec2f& WorldPos)	{	mWorldPath.PushTail( WorldPos );	}
	virtual ofColour	GetColour() const					{	return ofColour(200,200,200);	}
	virtual void		Render(float TimeStep,const TRenderSettings& RenderSettings);

public:
	ofShapePath2		mWorldPath;	//	path stored in world space
};

class TActorAutoPath : public TActorPathBase<TActors::AutoPath>
{
public:
	TActorAutoPath(TWorld& World)	{}

	virtual bool		Update(float TimeStep,TWorld& World);
};


class TActorDragPath : public TActorPathBase<TActors::DragPath>
{
public:
	TActorDragPath(TWorld& World)	{}
};





template<TActors::Type ACTORTYPE>
void TActorPathBase<ACTORTYPE>::Render(float TimeStep,const TRenderSettings& RenderSettings)
{
	TRenderSceneScope Scene(__FUNCTION__);
	ofSetColor( GetColour() );
	ofSetLineWidth( PATH_WIDTH );
	ofFill();

	//	rendering in world space
	auto& Points = mWorldPath.mPath;
	for ( int i=0;	i<Points.GetSize()-1;	i++ )
	{
		vec3f Start( Points[i].x, Points[i].y, GetZ() );
		vec3f End( Points[i+1].x, Points[i+1].y, GetZ() );
		ofLine( Start, End );
	}
}
