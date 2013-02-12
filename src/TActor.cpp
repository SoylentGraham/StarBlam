#include "TActor.h"



#define ROCKET_MIN_SPEED	400.f	//	unit/sec
#define ROCKET_MAX_SPEED	400.f	//	unit/sec
#define EXPLOSION_GROW_RATE	200.f	//	unit/sec
#define EXPLOSION_MAX_SIZE	40.f


//ofShapeBox3 gWorldBox( vec3f(-1000,-600,0), vec3f(1000,600,1000) );
ofShapeBox3 gWorldBox( vec3f(-WORLD_WIDTH/2,-300,4), vec3f(WORLD_WIDTH/2,300,1000) );



bool TActor::Update(float TimeStep,TWorld& World)
{
	return true;
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
	Shape.mCircle = ofShapeCircle2( mPosition, DEATHSTAR_SIZE );
	return Shape;
}



TActorStars::TActorStars() :
	TActorDerivitive	( STARS_Z )
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
	TActorDerivitive	( DRAG_Z )
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

void TActorDrag::SetLine(const ofLine2& Line)
{
	//	todo: update collision shape etc
	mLine = Line;
}




TActorRocket::TActorRocket(const ofLine2& FiringLine,TRef PlayerRef) :
	TActorDerivitive	( ROCKET_Z ),
	mVelocity			( FiringLine.mEnd - FiringLine.mStart ),
	mPlayerRef			( PlayerRef )
{
	//	convert velocity to units/per sec
	float VelSpeed = mVelocity.length();
	ofLimit( VelSpeed, ROCKET_MIN_SPEED, ROCKET_MAX_SPEED );
	mVelocity.normalize();
	mVelocity *= VelSpeed;


	SetPosition( FiringLine.mStart );
}


bool TActorRocket::Update(float TimeStep,TWorld& World)
{
	vec2f Velocity = mVelocity;

	//	add gravity forces

	//	move!
	mPosition.x += Velocity.x * TimeStep;
	mPosition.y += Velocity.y * TimeStep;

	//	die if out of world
	if ( gWorldBox.IsOutside( mPosition ) )
		return false;

	return true;
}

TCollisionShape TActorRocket::GetCollisionShape() const
{
	TCollisionShape Shape;
	Shape.mCircle = ofShapeCircle2( mPosition, ROCKET_SIZE );
	return Shape;
}


TActorExplosion::TActorExplosion(const vec2f& Position) :
	TActorDerivitive	( EXPLOSION_Z ),
	mSize				( 2.f )
{
	SetPosition( Position );
}

void TActorExplosion::Render(float TimeStep,const TRenderSettings& RenderSettings)
{
	mSize += EXPLOSION_GROW_RATE * TimeStep;

	TActor::Render( TimeStep, RenderSettings );
}
	
TCollisionShape TActorExplosion::GetCollisionShape() const
{
	TCollisionShape Shape;
	Shape.mCircle = ofShapeCircle2( GetPosition2(), mSize );
	return Shape;
}

bool TActorExplosion::Update(float TimeStep,TWorld& World)
{
	//	die when too big
	if ( mSize > EXPLOSION_MAX_SIZE )
		return false;

	return true;
}


TActorSentry::TActorSentry(const ofShapeCircle2& Shape,const ofColour& Colour) :
	TActorDerivitive	( SENTRY_Z ),
	mColour				( Colour ),
	mRadius				( Shape.mRadius )
{
	SetPosition( Shape.mPosition );
}


TCollisionShape TActorSentry::GetCollisionShape() const
{
	TCollisionShape Shape;
	Shape.mCircle = ofShapeCircle2( GetPosition2(), mRadius );
	return Shape;
}


