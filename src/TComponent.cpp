#include "TComponent.h"
#include "TActor.h"



void TComTransform::Render(const TRenderSettings& RenderSettings,const TTransform& ParentTransform,const TMaterial& Material)
{
	TTransform WorldTransform = GetTransform();
	ParentTransform.Transform( WorldTransform );

	//	render a box at the center...
	ofSetColor( Material.mColour );
	ofBox( WorldTransform.mPosition.x, WorldTransform.mPosition.y, Material.mZ, 4.f );

	//	and it's direction
	vec2f Normal = ofNormalFromAngle( WorldTransform.GetRotationDeg() );
	Normal += WorldTransform.mPosition;
	ofLine(	WorldTransform.mPosition.x, WorldTransform.mPosition.y, Material.mZ,
			Normal.x, Normal.y, Material.mZ );

}


void TComCollision::Render(const TRenderSettings& RenderSettings,const TCollisionShape& WorldShape,const TMaterial& Material)
{
	if ( !WorldShape.IsValid() )
		return;

	if ( Material.mOutline )
		ofNoFill();
	else
		ofFill();
	ofSetColor( Material.mColour );

	//	if the polygon is valid, that takes precendent
	if ( WorldShape.mPolygon.IsValid() )
	{
		vec3f a( WorldShape.mPolygon.mTriangle[0], Material.mZ );
		vec3f b( WorldShape.mPolygon.mTriangle[1], Material.mZ );
		vec3f c( WorldShape.mPolygon.mTriangle[2], Material.mZ );
		ofTriangle( a, b, c );
	}
	else if ( WorldShape.mCircle.IsValid() )
	{
		vec3f Pos( WorldShape.mCircle.mPosition, Material.mZ );
		ofCircle( Pos, WorldShape.mCircle.mRadius );
	}
	else
	{
		//	not valid at all?
	}

}

