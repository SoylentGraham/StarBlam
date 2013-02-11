#include "TActor.h"



#define MIN_ROCKET_SPEED	100.f	//	unit/sec
#define MAX_ROCKET_SPEED	400.f	//	unit/sec

//ofShapeBox3 gWorldBox( vec3f(-1000,-600,0), vec3f(1000,600,1000) );
ofShapeBox3 gWorldBox( vec3f(-WORLD_WIDTH/2,-300,4), vec3f(WORLD_WIDTH/2,300,1000) );



void TActor::Update(float TimeStep,TWorld& World)
{
}


void TActor::RenderCollision(const TRenderSettings& RenderSettings)
{
	//	get the collision shape
	TCollisionShape Shape = GetCollisionShape();
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
		ofCircle( CirclePos.x, CirclePos.y, mPosition.z, Shape.mCircle.mRadius );
	}

	//	render center
	{
		auto& Center = Shape.GetCenter();
		ofSetColor( ofColour(255), 1.0f );
		ofBox( Center.x, Center.y, mPosition.z, 4.f );
	}
}



void TActor::Render(float TimeStep,const TRenderSettings& RenderSettings)
{
	//	default renders the collision shape
	RenderCollision( RenderSettings );
}


TCollisionShape	TActorDeathStar::GetCollisionShape() const
{
	TCollisionShape Shape;
	Shape.mCircle = ofShapeCircle2( mPosition, 100.f );
	return Shape;
}



TActorStars::TActorStars() :
	TActor	( STARS_Z )
{
	//	generate random stars
	for ( int i=0;	i<200;	i++ )
	{
		auto& StarPos = mStars.PushBack();
		StarPos = gWorldBox.GetRandomPosInside();
	}
}


void TActorStars::Render(float TimeStep,const TRenderSettings& RenderSettings)
{
	TRenderSceneScope Scene(__FUNCTION__);

	ofFill();
	for ( int i=0;	i<mStars.GetSize();	i++ )
	{
		auto& Star = mStars[i];

		//	no perspective? scale
		static float MinRadius = 1.f;
		static float MaxRadius = 4.f;
		float Radius = MaxRadius;
		float DepthNormal = 1.f - gWorldBox.GetTimeZ( Star.z );
		Radius *= DepthNormal;
		Radius = ofMax( Radius, MinRadius );
		static float MinAlpha = 0.2f;
		static float MaxAlpha = 1.0f;
		float Alpha = ofLerp( MaxAlpha, MinAlpha, DepthNormal );
		int Componenet = static_cast<int>( ofLerp( 60, 200, DepthNormal ) );
		ofSetColor( Componenet, Componenet, Componenet, Alpha );

		ofCircle( Star.x, Star.y, mPosition.z, Radius );
	}
}



TActorDrag::TActorDrag() :
	TActor	( DRAG_Z )
{
}


void TActorDrag::Render(float TimeStep,const TRenderSettings& RenderSettings)
{
	TRenderSceneScope Scene(__FUNCTION__);

	ofFill();
	ofSetLineWidth( 10.f );
	ofSetColor( GetColour() );

	vec3f Start( mLine.mStart.x, mLine.mStart.y, mPosition.z );
	vec3f End( mLine.mEnd.x, mLine.mEnd.y, mPosition.z );
	ofLine( Start.x, Start.y, Start.z, 
			End.x, End.y, End.z );
}

void TActorDrag::SetLine(const ofShapeLine2& Line)
{
	//	todo: update collision shape etc
	mLine = Line;
}




TActorRocket::TActorRocket(const ofShapeLine2& FiringLine) :
	TActor		( ROCKET_Z ),
	mVelocity	( FiringLine.mEnd - FiringLine.mStart )
{
	//	convert velocity to units/per sec
	float VelSpeed = mVelocity.length();
	ofLimit( VelSpeed, MIN_ROCKET_SPEED, MAX_ROCKET_SPEED );
	mVelocity.normalize();
	mVelocity *= VelSpeed;


	SetPosition( FiringLine.mStart );
}


void TActorRocket::Update(float TimeStep,TWorld& World)
{
	vec2f Velocity = mVelocity;

	//	add gravity forces

	//	move!
	mPosition.x += Velocity.x * TimeStep;
	mPosition.y += Velocity.y * TimeStep;
}

TCollisionShape TActorRocket::GetCollisionShape() const
{
	TCollisionShape Shape;
	Shape.mCircle = ofShapeCircle2( mPosition, 20.f );
	return Shape;
}

