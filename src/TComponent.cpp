#include "TComponent.h"
#include "TActor.h"



void TComCollision::Render(const TRenderSettings& RenderSettings,const TTransform& WorldTransform)
{
	/*
	//	get the collision shape
	TCollisionShape Shape = GetWorldCollisionShape();
	if ( !Shape.IsValid() )
		return;

	TRenderSceneScope Scene(__FUNCTION__);

	//	render the circle
	if ( Shape.mCircle.IsValid() )
	{
		ofFill();
		ofSetColor( GetColour(), 0.5f );

		auto& CirclePos = Shape.mCircle.mPosition;

		//	render collision shape
		ofCircle( CirclePos.x, CirclePos.y, GetZ(), Shape.mCircle.mRadius );
	}

	//	render center
	{
		auto& Center = Shape.GetCenter();
		ofSetColor( ofColour(255), 1.0f );
		ofBox( Center.x, Center.y, GetZ(), 4.f );
	}
*/
}

