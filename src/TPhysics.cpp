#include "TWorld.h"



bool SoyPhysics::GetIntersection(const ofShapeCircle2& ShapeA,const ofShapeCircle2& ShapeB,TIntersection& Intersection)
{
	vec2f Delta = ShapeB.mPosition - ShapeA.mPosition;
	float DistSq = Delta.lengthSquared();

	float RadTotal = ShapeA.mRadius + ShapeB.mRadius;
	if ( DistSq > RadTotal*RadTotal )
		return false;
	
	Intersection.mIsValid = true;
	Intersection.mMidIntersection = ShapeA.mPosition + (Delta*0.5f);
	Intersection.mCollisionPointA = ShapeA.mPosition + ( Delta.normalized() * ShapeA.mRadius );
	Intersection.mCollisionPointB = ShapeB.mPosition + ( Delta.normalized() * -ShapeB.mRadius );

	return true;
}
