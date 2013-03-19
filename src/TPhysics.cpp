#include "TWorld.h"




bool SoyPhysics::GetIntersection(const TCollisionShape& ShapeA,const TCollisionShape& ShapeB,TIntersection& Intersection)
{
	//	what do we have to work with...
	auto& PolygonA = ShapeA.GetPolygon();
	auto& CapsuleA = ShapeA.GetCapsule();
	auto& CircleA = ShapeA.GetCircle();

	if ( PolygonA.IsValid() )
		return GetIntersection( PolygonA, ShapeB, Intersection );

	if ( CapsuleA.IsValid() )
		return GetIntersection( CapsuleA, ShapeB, Intersection );

	if ( CircleA.IsValid() )
		return GetIntersection( CircleA, ShapeB, Intersection );

	return false;
}

