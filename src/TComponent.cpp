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


void TComTransform::Render(const TRenderSettings& RenderSettings,const TTransform2& ParentTransform,const TMaterial& Material)
{
	auto WorldTransform = GetTransform();
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

	auto& Polygon = WorldShape.GetPolygon();
	auto& Capsule = WorldShape.GetCapsule();
	auto& Circle = WorldShape.GetCircle();

	//	if the polygon is valid, that takes precendent
	if ( Polygon.IsValid() )
	{
		vec3f a( Polygon.mTriangle[0], Material.mZ );
		vec3f b( Polygon.mTriangle[1], Material.mZ );
		vec3f c( Polygon.mTriangle[2], Material.mZ );
		ofTriangle( a, b, c );
	}
	else if ( Capsule.IsValid() )
	{
		//	generate triangle points
		float mRadius = Capsule.mRadius;
		ofLine2 Line = Capsule.mLine;
		vec2f Normal = Line.GetNormal();
		vec2f Right = Normal.getPerpendicular();
		float z = Material.mZ;

		//	BL TL TR BR (bottom = start)
		vec3f Quad_BL( Line.mStart - (Right*mRadius), z );
		vec3f Quad_TL( Line.mEnd - (Right*mRadius), z );
		vec3f Quad_TR( Line.mEnd + (Right*mRadius), z );
		vec3f Quad_BR( Line.mStart + (Right*mRadius), z );

		ofLine( Quad_BL, Quad_TL );
		ofLine( Quad_BR, Quad_TR );
		ofLine( Line.mStart, Line.mEnd );
		ofRect( Line.mStart, mRadius/4.f, mRadius/4.f );
		//ofCircle( Line.mStart, mRadius );
		//ofCircle( Line.mEnd, mRadius );

		//	http://digerati-illuminatus.blogspot.co.uk/2008/05/approximating-semicircle-with-cubic.html
		vec3f BLControl = Quad_BL - (Normal * mRadius * 4.f/3.f) + (Right * mRadius * 0.10f );
		vec3f BRControl = Quad_BR - (Normal * mRadius * 4.f/3.f) - (Right * mRadius * 0.10f );
		vec3f TLControl = Quad_TL + (Normal * mRadius * 4.f/3.f) + (Right * mRadius * 0.10f );
		vec3f TRControl = Quad_TR + (Normal * mRadius * 4.f/3.f) - (Right * mRadius * 0.10f );
		BLControl.z = 
		BRControl.z =
		TLControl.z = 
		TRControl.z = z;
		ofBezier( Quad_BL, BLControl, BRControl, Quad_BR );
		ofBezier( Quad_TL, TLControl, TRControl, Quad_TR );
	}
	else if ( Circle.IsValid() )
	{
		vec3f Pos( Circle.mPosition, Material.mZ );
		ofCircle( Pos, Circle.mRadius );
	}
	else
	{
		//	not valid at all?
	}

}




ofShapeCircle2 TComGravity::GetWorldGravityShape(const TTransform2& WorldTransform)
{
	//	if our shape isn't valid, then see if we can generate it from the collision shape
	if ( !mLocalShape.IsValid() )
	{
		//	base it on the collision shape
		auto* pCollision = GetComponent<TComCollision>();
		if ( pCollision )
			SetLocalGravityShape( pCollision->mShape.GetCircle() );
	}

	ofShapeCircle2 Circle = mLocalShape;
	Circle.Transform( WorldTransform );
	Circle.mRadius = Circle.mRadius*2.f;
	
	return Circle;
}

void TComGravity::Render(const TRenderSettings& RenderSettings,const TTransform2& WorldTransform,const TMaterial& Material)
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



