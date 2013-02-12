#pragma once

#include "Main.h"
#include "TActor.h"
#include "ofShape.h"



class TCollisionActor
{
public:
	TCollisionActor() :
		mActor		( NULL )
	{
	}
	TCollisionActor(TActor& Actor) :
		mActor	( &Actor ),
		mShape	( Actor.GetCollisionShape() )
	{
	}

	bool			IsValid() const		{	return mShape.IsValid();	}

public:
	TCollisionShape	mShape;
	TActor*			mActor;
};


class TIntersection
{
public:
	TIntersection() :
		mIsValid		( false )
	{
	}

	bool		IsValid() const			{	return mIsValid;	}

public:
	bool		mIsValid;
	vec2f		mMidIntersection;
	vec2f		mCollisionPointA;
	vec2f		mCollisionPointB;
};

class TCollisionTest
{
public:
	TCollisionTest() :
		mActorA		( NULL ),
		mActorB		( NULL )
	{
	}
	TCollisionTest(TCollisionActor& ActorA,TCollisionActor& ActorB) :
		mActorA		( &ActorA ),
		mActorB		( &ActorB )
	{
	}

public:
	TCollisionActor*		mActorA;
	TCollisionActor*		mActorB;
	BufferArray<vec2f,2>	mCollisionPoints;
};

class TCollision
{
public:
	TCollision()
	{
	}
	TCollision(const TCollisionTest& Collision,const TIntersection& Intersection) :
		mActorA			( *Collision.mActorA->mActor ),
		mActorB			( *Collision.mActorB->mActor ),
		mIntersection	( Intersection )
	{
	}

	bool			IsValid() const						{	return mActorA.IsValid() && mActorB.IsValid() && mIntersection.IsValid();	}

public:
	TIntersection	mIntersection;
	TActorRef		mActorA;
	TActorRef		mActorB;
};


namespace SoyPhysics
{
	bool	GetIntersection(const ofShapeCircle2& ShapeA,const ofShapeCircle2& ShapeB,TIntersection& Intersection);
};

