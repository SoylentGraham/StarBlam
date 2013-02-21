#include "TComponent.h"
#include "TActor.h"


//	global definition
Array<TComContainerBase*> TComs::Private::gContainerRegistry;



TComContainerBase::TComContainerBase()
{
	TComs::Private::gContainerRegistry.PushBackUnique( this );
}

TComContainerBase::~TComContainerBase()
{
	TComs::Private::gContainerRegistry.Remove( this );
}


void TComs::DestroyComponents(const TActorRef& Ref)
{
	//	go over the component registery and remove any components for this actor
	for ( int i=0;	i<Private::gContainerRegistry.GetSize();	i++ )
	{
		auto& Container = *Private::gContainerRegistry[i];
		Container.Destroy( Ref );
	}
}


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




ofShapeCircle2 TComGravity::GetWorldGravityShape(const TTransform& WorldTransform)
{
	//	if our shape isn't valid, then see if we can generate it from the collision shape
	if ( !mLocalShape.IsValid() )
	{
		//	base it on the collision shape
		auto* pCollision = GetComponent<TComCollision>();
		if ( pCollision )
			SetLocalGravityShape( pCollision->mShape.mCircle );
	}

	ofShapeCircle2 Circle = mLocalShape;
	Circle.Transform( WorldTransform );
	Circle.mRadius = Circle.mRadius*2.f;
	
	return Circle;
}

void TComGravity::Render(const TRenderSettings& RenderSettings,const TTransform& WorldTransform,const TMaterial& Material)
{
	//	render gravity field
	ofShapeCircle2 GravityCircle = GetWorldGravityShape( WorldTransform );
	if ( !GravityCircle.IsValid() )
		return;

	ofNoFill();
	ofSetColor( ofColour(255,255,255) );

	vec3f Pos( GravityCircle.mPosition, Material.mZ );
	ofCircle( Pos, GravityCircle.mRadius );
}

void TComGravity::SetLocalGravityShape(const ofShapeCircle2& BoundsShape)
{
	mLocalShape = BoundsShape;	
}



