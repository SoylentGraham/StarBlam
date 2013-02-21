#include "TWorld.h"



template<class SHAPEA,class SHAPEB>
bool GetIntersection(const SHAPEA& ShapeA,const SHAPEB& ShapeB,TIntersection& Intersection)
{
	Intersection = ofShape::GetIntersection( ShapeA, ShapeB );
	return Intersection;
}



bool SoyPhysics::GetIntersection(const TCollisionShape& ShapeA,const TCollisionShape& ShapeB,TIntersection& Intersection)
{
	//	what do we have to work with...
	if ( ShapeA.mPolygon.IsValid() )
	{
		if ( ShapeB.mPolygon.IsValid() )
		{
			return GetIntersection( ShapeA.mPolygon, ShapeB.mPolygon, Intersection );
		}
		else if ( ShapeB.mCircle.IsValid() )
		{
			return GetIntersection( ShapeA.mPolygon, ShapeB.mCircle, Intersection );
		}
	}
	else if ( ShapeA.mCircle.IsValid() )
	{
		if ( ShapeB.mPolygon.IsValid() )
		{
			return GetIntersection( ShapeB.mPolygon, ShapeA.mCircle, Intersection );
		}
		else if ( ShapeB.mCircle.IsValid() )
		{
			return GetIntersection( ShapeA.mCircle, ShapeB.mCircle, Intersection );
		}
	}

	return false;
}
