#include "TWorld.h"




bool SoyPhysics::GetIntersection(const ofShapeCircle2& ShapeA,const ofShapeCircle2& ShapeB,TIntersection& Intersection)
{
	TIntersection2 Intersection2 = ShapeA.GetIntersection( ShapeB );
	if ( !Intersection2 )
		return false;

	auto& Delta = Intersection2.mDelta;

	Intersection.mIsValid = true;
	Intersection.mMidIntersection = ShapeA.mPosition + ( Delta*0.5f);
	Intersection.mCollisionPointA = ShapeA.mPosition + ( Delta.normalized() * ShapeA.mRadius );
	Intersection.mCollisionPointB = ShapeB.mPosition + ( Delta.normalized() * -ShapeB.mRadius );

	return true;
}
