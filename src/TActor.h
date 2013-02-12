#pragma once

#include "Main.h"
#include "TRender.h"

class TWorld;	//	cyclic dependancy!
class TActor;

#define WORLD_WIDTH	1000

#define STARS_Z		1
#define DEATHSTAR_Z	2
#define ROCKET_Z	3
#define EXPLOSION_Z	4
#define GUI_Z		5
#define DRAG_Z		GUI_Z



namespace TActors
{
	enum Type
	{
		DeathStar,
		Stars,
		Rocket,
		Drag,
		Explosion,
	};
};

//---------------------------------
//	change this to a real ref later
//---------------------------------
class TActorRef
{
public:
	TActorRef() :
		mActor	( NULL )
	{
	}
	TActorRef(const TActor& Actor) :
		mActor	( &Actor )
	{
	}
	
	bool			IsValid() const							{	return mActor != NULL;	}
	inline bool		operator==(const TActor& Actor) const	{	return mActor == &Actor;	}

public:
	const TActor*	mActor;
};

class TCollisionShape
{
public:
	virtual bool		IsValid() const		{	return mCircle.mRadius > 0.f;	}
	virtual vec2f		GetCenter() const	{	return mCircle.mPosition;	}

public:
	ofShapeCircle2		mCircle;
};


class TActor
{
public:
	TActor(float z) :
		mPosition	( 0, 0, z )
	{
	}

	virtual TActors::Type	GetType() const=0;
	TActorRef				GetRef() const					{	return TActorRef( *this );	}
	virtual void			Render(float TimeStep,const TRenderSettings& RenderSettings);
	virtual void			RenderCollision(const TRenderSettings& RenderSettings);
	virtual bool			Update(float TimeStep,TWorld& World);		//	return false to die
	
	vec3f					GetPosition3() const 			{	return mPosition;	}
	vec2f					GetPosition2() const 			{	return vec2f( GetPosition3().x, GetPosition3().y );	}
	void					SetPosition(const vec2f& Pos)	{	mPosition.x = Pos.x;	mPosition.y = Pos.y;	}
	virtual ofColour		GetColour() const				{	return ofColour(255,255,0);	}
	virtual TCollisionShape	GetCollisionShape() const		{	return TCollisionShape();	}

	inline bool				operator==(const TActor& Actor) const		{	return this == &Actor;	}
	inline bool				operator==(const TActors::Type& Type) const	{	return GetType() == Type;	}

public:
	vec3f					mPosition;
};



template<TActors::Type ACTORTYPE>
class TActorDerivitive : public TActor
{
public:
	static const TActors::Type TYPE = ACTORTYPE;

public:
	TActorDerivitive(float z) :
		TActor	( z )
	{
	}
	virtual TActors::Type	GetType() const	{	return ACTORTYPE;	}
};

class TActorDeathStar : public TActorDerivitive<TActors::DeathStar>
{
public:
	TActorDeathStar() :
		TActorDerivitive	( DEATHSTAR_Z )
	{
	}

	virtual ofColour		GetColour() const			{	return mMaterial.mColour;	}
	virtual TCollisionShape	GetCollisionShape() const;

public:
	TMaterial			mMaterial;
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
	TActorDrag();

	virtual void		Render(float TimeStep,const TRenderSettings& RenderSettings);

	virtual ofColour	GetColour() const			{	return ofColour( 230,10,10 );	}
	void				SetLine(const ofShapeLine2& Line);

public:
	ofShapeLine2		mLine;
};



class TActorRocket : public TActorDerivitive<TActors::Rocket>
{
public:
	TActorRocket(const ofShapeLine2& FiringLine,TRef PlayerRef);

	virtual bool			Update(float TimeStep,TWorld& World);
	
	virtual TCollisionShape	GetCollisionShape() const;

public:
	TRef				mPlayerRef;		//	player which fired this rocket so we can ignore collisions from the source player (ie. at firing time)
	vec2f				mVelocity;
};



class TActorExplosion : public TActorDerivitive<TActors::Explosion>
{
public:
	TActorExplosion(const vec2f& Position);

	virtual void			Render(float TimeStep,const TRenderSettings& RenderSettings);
	virtual bool			Update(float TimeStep,TWorld& World);
	
	virtual TCollisionShape	GetCollisionShape() const;

public:
	float					mSize;
};

