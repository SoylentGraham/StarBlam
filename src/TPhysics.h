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
		mShape	( Actor.GetWorldCollisionShape() )
	{
	}

	bool			IsValid() const		{	return mShape.IsValid();	}

public:
	TCollisionShape	mShape;
	TActor*			mActor;
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
	bool	GetIntersection(const TCollisionShape& ShapeA,const TCollisionShape& ShapeB,TIntersection& Intersection);

	template<class SHAPEA,class SHAPEB>
	inline bool GetIntersection(const SHAPEA& ShapeA,const SHAPEB& ShapeB,TIntersection& Intersection)
	{
		Intersection = ofShape::GetIntersection( ShapeA, ShapeB );
		return Intersection;
	}

	template<class SHAPEA>
	inline bool GetIntersection(const SHAPEA& ShapeA,const TCollisionShape& ShapeB,TIntersection& Intersection)
	{
		//	what do we have to work with...
		auto& PolygonB = ShapeB.GetPolygon();
		auto& CapsuleB = ShapeB.GetCapsule();
		auto& CircleB = ShapeB.GetCircle();

		if ( PolygonB.IsValid() )
			return GetIntersection( ShapeA, PolygonB, Intersection );
	
		if ( CapsuleB.IsValid() )
			return GetIntersection( ShapeA, CapsuleB, Intersection );
	
		if ( CircleB.IsValid() )
			return GetIntersection( ShapeA, CircleB, Intersection );

		return false;
	}
};


