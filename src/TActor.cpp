#include "TActor.h"
#include "TWorld.h"



//ofShapeBox3 gWorldBox( vec3f(-1000,-600,0), vec3f(1000,600,1000) );
ofShapeBox3 gWorldBox( vec3f(-WORLD_WIDTH/2,-300,4), vec3f(WORLD_WIDTH/2,300,1000) );


TActor::TActor(const TActorMeta& Meta)
{
	//	if initialised with an owner, set it up
	if ( Meta.mOwnerPlayer.IsValid() )
	{
		AddComponent<TComOwnerPlayer>( Meta.mOwnerPlayer );
	}
}

//	accumulate the collision shape of all children in the tree, localised to this
ofShapeCircle2 TActor::GetTreeLocalCollisionShape(TWorld& World) const
{
	//	start with our shape...
	ofShapeCircle2 Shape = GetLocalCollisionShape().mCircle;

	//	accumulate children's shapes
	Array<TActorRef> Children = GetChildren();
	for ( int i=0;	i<Children.GetSize();	i++ )
	{
		auto* pChild = World.GetActor( Children[i] );
		if ( !pChild )
			continue;

		ofShapeCircle2 ChildShape = pChild->GetTreeLocalCollisionShape( World );
		if ( !ChildShape.IsValid() )
			continue;
		Shape.Accumulate( ChildShape );
	}

	return Shape;
}

TRef TActor::GetOwnerPlayer() const
{
	//	look for componenent...
	auto* pOwnerPlayer = GetComponent<TComOwnerPlayer>();
	if ( pOwnerPlayer )
		return pOwnerPlayer->mPlayer;
	else
		return TRef();
}


TActor::~TActor()
{
	//	if we still have a parent, this actor hasn't been destroyed from the world...
	assert( !mParent.IsValid() );

	//	destroy components
	TComs::DestroyComponents( GetRef() );
}


bool TActor::HasChild(TActorRef ChildRef) const
{
	//	look down the heirachy for this actor
	Array<TActorRef> Children = GetChildren();
	for ( int i=0;	i<Children.GetSize();	i++ )
	{
		auto& Child = Children[i];
		if ( Child == ChildRef )
			return true;

		//	look down heirachy
		auto* ChildActor = Child.GetActor();
		if ( !ChildActor )
			continue;
		if ( ChildActor->HasChild( ChildRef ) )
			return true;
	}

	return false;
}

void TActor::SetParent(TActorRef Parent,TWorld& World)
{
	//	shouldnt overwrite parent unless it's with no-parent
	if ( Parent.IsValid() )
	{
		assert( !mParent.IsValid() );
	}
	TActorRef OldParent = mParent;
	mParent = Parent;

	//	ensure the tree is valid...
	if ( Parent.IsValid() )
	{
		auto* pParentActor = Parent.GetActor();
		auto Children = pParentActor->GetChildren();
		auto ChildRef = GetRef();
		assert( Children.Find( ChildRef ) );
	}
	else if ( OldParent.IsValid() )
	{
		//	disconnecting from parent
		auto* pParent = World.GetActor( OldParent );
		if ( pParent )
		{
			//	get transform
			//TTransform ParentTransform = pParent->GetWorldTransform();
			pParent->OnChildReleased( GetRef(), World );
		}
	}
}


TTransform TActor::GetParentWorldTransform() const
{
	TTransform Transform;

	//	get parent's transform
	const TActor* pParent = mParent.GetActor();
	if ( pParent )
	{
		TTransform ParentTransform = pParent->GetWorldTransform();
		ParentTransform.Transform( Transform );
	}

	return Transform;
}

TTransform TActor::GetWorldTransform() const
{
	//	get local transform
	TTransform Transform;
	auto* pTransform = GetComponent<TComTransform>();
	if ( pTransform )
		Transform = pTransform->GetTransform();

	//	get parent's transform
	TTransform ParentTransform = GetParentWorldTransform();
	ParentTransform.Transform( Transform );

	return Transform;
}


TCollisionShape TActor::GetWorldCollisionShape() const
{
	TCollisionShape LocalShape = GetLocalCollisionShape();
	TTransform Transform = GetParentWorldTransform();
	LocalShape.Transform( Transform );
	return LocalShape;
}


TCollisionShape TActor::GetLocalCollisionShape() const
{
	//	get the collision shape
	auto* pCollision = GetComponent<TComCollision>();
	if ( !pCollision )
		return TCollisionShape();

	//	check if there's a transform to apply to the shape...
	auto* pTransform = GetComponent<TComTransform>();	
	if ( pTransform )
	{
		//	transform
		TCollisionShape Shape = pCollision->mShape;
		Shape.Transform( pTransform->GetTransform() );
		return Shape;
	}

	return pCollision->mShape;
}

float TActor::GetZ() const
{
	switch ( GetType() )
	{
		case TActors::DeathStar:		return DEATHSTAR_Z;
		case TActors::Stars:			return STARS_Z;
		case TActors::Rocket:			return ROCKET_Z;
		case TActors::Drag:				return GUI_Z;
		case TActors::AutoPath:			return GUI_Z;
		case TActors::Explosion:		return EXPLOSION_Z;
		case TActors::Sentry_Rocket:	return SENTRY_Z;
		case TActors::Sentry_LaserBeam:	return SENTRY_Z;
		case TActors::Sentry_Rotation:	return SENTRY_Z;
		case TActors::LaserBeam:		return LASERBEAM_Z;
		case TActors::Asteroid:			return ASTEROID_Z;
		case TActors::AsteroidChunk:	return ASTEROID_Z;
	};

	//	unhandled case
	assert(false);
	return 0.f;
}


void TActor::SetWorldRotation(float AngleDeg)
{
	//	need to compensate for parent's rotation
	TTransform ParentTransform = GetParentWorldTransform();
	AngleDeg -= ParentTransform.GetRotationDeg();

	//	grab transform to change
	auto* pTransform = GetComponent<TComTransform>();
	if ( !pTransform )
	{
		//	trying to transform an actor with no transform component
		assert( pTransform );
		return;
	}

	pTransform->SetRotationDeg( AngleDeg );
}

void TActor::SetLocalPosition(const vec2f& Pos)	
{
	auto* pTransform = GetComponent<TComTransform>();
	if ( !pTransform )
	{
		//	trying to transform an actor with no position
		assert( pTransform );
		return;
	}

	pTransform->mPosition = Pos;
}


vec2f TActor::GetLocalPosition2() const 			
{
	//	look for transform component
	auto* pTransform = GetComponent<TComTransform>();
	if ( pTransform )
	{
		return pTransform->mPosition;
	}

	//	use alternative components... like collison?

	return vec2f(0,0);
}

vec2f TActor::GetWorldPosition2() const 			
{
	vec2f LocalPosition = GetLocalPosition2();
	TTransform WorldTransform = GetParentWorldTransform();
	WorldTransform.Transform( LocalPosition );
	return LocalPosition;
}


bool TActor::Update(float TimeStep,TWorld& World)
{
	return true;
}


void TActor::Render(float TimeStep,const TRenderSettings& RenderSettings)
{
	TRenderSceneScope Scene(__FUNCTION__);
	TMaterial Material( GetZ(), GetColour(), true );

	//	grab collision component and render it
	auto* pCollision = GetComponent<TComCollision>();
	if ( pCollision )
	{
		pCollision->Render( RenderSettings, GetWorldCollisionShape(), Material );
	}

	//	render transform
	auto* pTransform = GetComponent<TComTransform>();
	if ( pTransform )
	{
		pTransform->Render( RenderSettings, GetParentWorldTransform(), Material );
	}

	//	render grvaity
	auto* pGravity = GetComponent<TComGravity>();
	if ( pGravity )
	{
		pGravity->Render( RenderSettings, GetWorldTransform(), Material );
	}
}



TActorDeathStar::TActorDeathStar(const TActorMeta& Meta,TWorld& World) :
	TActorDerivitive	( Meta ),
	mColour				( Meta.mColour ),
	mInitialPos			( Meta.mPosition ),
	mOffset				( 0.f )
{
	AddComponent<TComGravity>();

	//	make a collisoin component
	auto& Collision = AddComponent<TComCollision>();
	Collision.mShape.mCircle = ofShapeCircle2( DEATHSTAR_SIZE );
	ofShapeCircle2 DeathStarShape = GetWorldCollisionShape().mCircle;
	assert( DeathStarShape.IsValid() );

	auto& Transform = AddComponent<TComTransform>();
	SetLocalPosition( Meta.mPosition );

	//	create child sentrys
	for ( int i=0;	i<SENTRY_COUNT;	i++ )
	{
		float AngleDeg = 360.f * ( static_cast<float>(i) / static_cast<float>( SENTRY_COUNT ) );
		
		//	position on edge of death star
		vec2f Normal = ofNormalFromAngle( AngleDeg );
		TActorMeta SentryMeta;
		SentryMeta.mPosition = Normal * (SENTRY_RADIUS + DeathStarShape.mRadius);
		SentryMeta.mColour = Meta.mColour;
		SentryMeta.mOwnerPlayer = Meta.mOwnerPlayer;
		SentryMeta.SetRotationDeg( AngleDeg );

		TActorSentry* pNewSentry = NULL;
		if ( i % 3 == 0 )
		{
			pNewSentry = World.CreateActor<TActorSentryLaserBeam>( SentryMeta );
		}
		else if ( i % 3 == 1 )
		{
			pNewSentry = World.CreateActor<TActorSentryRotation>( SentryMeta );
		}
		else
		{
			pNewSentry = World.CreateActor<TActorSentryRocket>( SentryMeta );
		}

		//	set heirachy
		mSentrys.PushBack( pNewSentry );
		pNewSentry->SetParent( GetRef(), World );
	}

}


void TActorDeathStar::GetSentrys(TWorld& World,Array<TActorSentry*>& Sentrys)
{
	for ( int i=0;	i<mSentrys.GetSize();	i++ )
	{
		TActorRef SentryRef = mSentrys[i];
		TActorSentry* pSentry = World.GetActor<TActorSentry>( SentryRef );
		Sentrys.PushBack( pSentry );
	}
}
	
Array<TActorRef> TActorDeathStar::GetChildren() const
{
	Array<TActorRef> Children;
	for ( int i=0;	i<mSentrys.GetSize();	i++ )
	{
		TActorRef SentryRef = mSentrys[i];
		Children.PushBack( SentryRef );
	}
	return Children;
}

bool TActorDeathStar::Update(float TimeStep,TWorld& World)
{
	//	testing heirachy code
	mOffset += TimeStep;
	if ( mOffset > 10 )
		mOffset -= 10*2;

	//	update transform
	auto* pTransform = GetComponent<TComTransform>();
	if ( pTransform )
	{
		/*
		pTransform->mPosition = mInitialPos;
		pTransform->mPosition.y += mOffset * 5;

		//	rotate
		float NewAngle = pTransform->GetRotationDeg();
		NewAngle += 60 * TimeStep;
		pTransform->SetRotationDeg( NewAngle );
		*/
	}

	return true;
}

	
void TActorDeathStar::OnChildReleased(TActorRef ChildRef,TWorld& World)
{
	mSentrys.Remove( ChildRef );
}


TActorStars::TActorStars(TWorld& World)
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

		ofCircle( Star.x, Star.y, GetZ(), Radius );
	}
}



void TActorDrag::Render(float TimeStep,const TRenderSettings& RenderSettings)
{
	TRenderSceneScope Scene(__FUNCTION__);

	ofFill();
	ofSetLineWidth( DRAG_WIDTH );
	ofSetColor( GetColour() );

	ofLine( mLine.mStart.x, mLine.mStart.y, GetZ(), 
			mLine.mEnd.x, mLine.mEnd.y, GetZ() );
}

void TActorDrag::SetLine(const ofLine2& Line)
{
	//	todo: update collision shape etc
	mLine = Line;
}



TActorRocket::TActorRocket(const ofLine2& FiringLine,const TActorMeta& ActorMeta,TWorld& World) :
	mVelocity			( FiringLine.mEnd - FiringLine.mStart ),
	TActorDerivitive	( ActorMeta )
{
	//	convert velocity to units/per sec
	float VelSpeed = mVelocity.length();
	ofLimit( VelSpeed, ROCKET_MIN_SPEED, ROCKET_MAX_SPEED );
	mVelocity.normalize();
	mVelocity *= VelSpeed;

	auto& Transform = AddComponent<TComTransform>();

	auto& Collision = AddComponent<TComCollision>();
	Collision.mShape.mCircle = ofShapeCircle2( ROCKET_SIZE );

	SetLocalPosition( FiringLine.mStart );

	//	show path
	auto* pPath = World.CreateActor<TActorAutoPath>();
	mAutoPath = pPath;
	pPath->SetParent( GetRef(), World );
}


bool TActorRocket::Update(float TimeStep,TWorld& World)
{
	//	apply gravity forces
	TCollisionShape RocketShape = GetWorldCollisionShape();
	auto& GravityComponents = TComs::GetContainer<TComGravity>().mComponents;
	TRef OwnerPlayer = GetOwnerPlayer();
	for ( int i=0;	i<GravityComponents.GetSize();	i++ )
	{
		auto& Gravity = *GravityComponents[i];
		//	skip any gravity components owned by the same player
		auto* pOwner = TComs::GetComponent<TComOwnerPlayer>( Gravity.GetRef() );
		if ( pOwner && OwnerPlayer == pOwner->mPlayer )
			continue;

		//	see if we're inside that gravity field
		auto* pActor = World.GetActor( Gravity.GetRef() );
		if ( !pActor )
			continue;
		ofShapeCircle2 GravityShape = Gravity.GetWorldGravityShape( pActor->GetWorldTransform() );
		if ( !GravityShape.IsValid() )
			continue;
		if ( !GravityShape.IsInside( RocketShape.mCircle ) )
			continue;
		
		//	draw in based on size of gravity field
		vec2f DirToGravity = GravityShape.GetCenter() - RocketShape.GetCenter();
		
		//float Force = ofGetMathTime( DirToGravity.length(), 0.f, GravityShape.mRadius );	//	inverse
		float Force = GravityShape.mRadius - DirToGravity.length();	//	inverse
		DirToGravity.normalize();
		Force *= GRAVITY_FORCE_FACTOR;
		
		mVelocity += DirToGravity * Force * TimeStep;
	}

	vec2f Velocity = mVelocity;


	//	move!
	auto* pTransform = GetComponent<TComTransform>();
	if ( pTransform )
	{
		pTransform->mPosition.x += Velocity.x * TimeStep;
		pTransform->mPosition.y += Velocity.y * TimeStep;

		//	die if out of world
		if ( gWorldBox.IsOutside( pTransform->mPosition ) )
			return false;
	}

	return true;
}

void TActorRocket::OnChildReleased(TActorRef ChildRef,TWorld& World)
{
	if ( mAutoPath == ChildRef )
		mAutoPath = TActorRef();
}

Array<TActorRef> TActorRocket::GetChildren() const
{
	Array<TActorRef> Children;
	if ( mAutoPath.IsValid() )
		Children.PushBack( mAutoPath );
	return Children;
}

void TActorRocket::OnPreDestroy(TWorld& World)
{
	//	disconnect the path so it persists
	if ( mAutoPath.IsValid() )
	{
		auto* pPath = World.GetActor( mAutoPath );
		if ( pPath )
		{
			pPath->SetParent( TActorRef(), World );
		}
	}
}





TActorExplosion::TActorExplosion(const vec2f& Position,TWorld& World)
{
	auto& Collision = AddComponent<TComCollision>();
	Collision.mShape.mCircle = ofShapeCircle2( EXPLOSION_INITIAL_SIZE );

	auto& Transform = AddComponent<TComTransform>();
	SetLocalPosition( Position );
}

bool TActorExplosion::Update(float TimeStep,TWorld& World)
{
	//	update collision size
	auto* Collision = GetComponent<TComCollision>();
	if ( Collision )
	{
		auto& Size = Collision->mShape.mCircle.mRadius;
		Size += EXPLOSION_GROW_RATE * TimeStep;

		//	die when too big
		if ( Size > EXPLOSION_MAX_SIZE )
			return false;
	}

	return true;
}


TActorSentry::TActorSentry(const TActorMeta& ActorMeta) :
	TActor				( ActorMeta ),
	mColour				( ActorMeta.mColour )
{
	auto& Transform = AddComponent<TComTransform>();
	Transform.GetTransform() = ActorMeta.GetTransform();

	//	make a collision component
	auto& Collision = AddComponent<TComCollision>();
	Collision.mShape.mCircle = ofShapeCircle2( SENTRY_RADIUS );
}



TActorSentryLaserBeam::TActorSentryLaserBeam(const TActorMeta& ActorMeta,TWorld& World) :
	TActorSentryBase	( ActorMeta )
{
	//	create laser beam
	TActorMeta LaserBeamMeta;
	LaserBeamMeta.mPosition = ActorMeta.mPosition;
	auto* pLaserBeam = World.CreateActor<TActorLaserBeam>( LaserBeamMeta );
	mLaserBeam = pLaserBeam;
	pLaserBeam->SetParent( GetRef(), World );
}


bool TActorSentryLaserBeam::Update(float TimeStep,TWorld& World)
{
	TActorLaserBeam* LaserBeam = World.GetActor<TActorLaserBeam>( mLaserBeam );

	if ( LaserBeam )
		LaserBeam->mActive = IsActive();

	return true;
}

void TActorSentryLaserBeam::SetState(TActorSentryState::Type State)
{
	mState = State;
}

Array<TActorRef> TActorSentryLaserBeam::GetChildren() const
{
	Array<TActorRef> Children;
	Children.PushBack( mLaserBeam );
	return Children;
}




TActorLaserBeam::TActorLaserBeam(const TActorMeta& ActorMeta,TWorld& World) :
	mActive				( false ),
	mLength				( LASERBEAM_MIN_LENGTH )
{
}

void TActorLaserBeam::Render(float TimeStep,const TRenderSettings& RenderSettings)
{
	TRenderSceneScope Scene(__FUNCTION__);
	ofFill();
	ofSetColor( GetColour(), 0.5f );
	
	//	generate triangle points
	float mWidth = LASERBEAM_WIDTH;
	ofLine2 Line = GetWorldBeamLine();
	vec2f Normal = Line.GetNormal();
	vec2f Right = Normal.getPerpendicular();

	float z = GetZ();
	BufferArray<vec3f,4> TrianglePoints;
	TrianglePoints.PushBack( vec3f( Line.mStart - (Right*mWidth), z ) );
	TrianglePoints.PushBack( vec3f( Line.mEnd - (Right*mWidth), z ) );
	TrianglePoints.PushBack( vec3f( Line.mEnd + (Right*mWidth), z ) );
	TrianglePoints.PushBack( vec3f( Line.mStart + (Right*mWidth), z ) );

	ofTriangle( TrianglePoints[0], TrianglePoints[1], TrianglePoints[2] );
	ofTriangle( TrianglePoints[2], TrianglePoints[3], TrianglePoints[0] );
}

bool TActorLaserBeam::Update(float TimeStep,TWorld& World)
{
	//	idle
	if ( !mActive && mLength <= LASERBEAM_MIN_LENGTH )
		return true;

	//	retract
	if ( !mActive )
	{
		//	retract
		mLength -= LASERBEAM_RETRACT_SPEED * TimeStep;
		mLength = ofMax( mLength, LASERBEAM_MIN_LENGTH );
		return true;
	}

	//	extend beam as neccessary
	TRaycastResult Raycast;
	vec2f Position = GetWorldPosition2();

	//	ray extends forever!
	ofLine2 BeamLine = GetWorldBeamLine();
	vec2f Normal = BeamLine.GetNormal();
	if ( !World.DoRaycast( TRaycast( BeamLine.mStart, Normal ), Raycast ) )
	{
		Raycast.mIntersection = Position + (Normal * 2000.f);
	}

	float EndLength = (Raycast.mIntersection - Position).length();

	//	extend
	if ( EndLength > mLength )
	{
		mLength += LASERBEAM_EXTEND_SPEED * TimeStep;
		mLength = ofMin( mLength, LASERBEAM_MAX_LENGTH );
	}

	//	stop short
	if ( EndLength <= mLength )
	{
		mLength = EndLength;
	}

	//	heat up & kill the actor
	return true;
}


ofLine2 TActorLaserBeam::GetWorldBeamLine() const	
{
	vec2f Pos = GetWorldPosition2();

	//	get rotation
	TTransform WorldTransform = GetWorldTransform();
	vec2f Normal = ofNormalFromAngle( WorldTransform.GetRotationDeg() );

	return ofLine2( Pos, Pos + (Normal*mLength) );	
}



void TActorSentryLaserBeam::OnChildReleased(TActorRef ChildRef,TWorld& World)
{
	if ( mLaserBeam == ChildRef )
		mLaserBeam = TActorRef();
}





TActorAsteroid::TActorAsteroid(const TActorMeta& ActorMeta,float Radius,TWorld& World)
{
	AddComponent<TComGravity>();

	auto& Transform = AddComponent<TComTransform>();
	Transform.GetTransform() = ActorMeta.GetTransform();

	//	generate local-space chunks
	Array<ofShapePolygon2> ChunkShapes;
	
	auto& First = ChunkShapes.PushBack();
	First.mTriangle.PushBack( vec2f( 0, -Radius ) );
	First.mTriangle.PushBack( vec2f( -Radius, Radius ) );
	First.mTriangle.PushBack( vec2f( Radius, Radius ) );

	for ( int i=0;	i<ChunkShapes.GetSize();	i++ )
	{
		auto& Shape = ChunkShapes[i];
		auto* pChunk = World.CreateActor<TActorAsteroidChunk>( Shape );
		if ( !pChunk )
			continue;
		mChunks.PushBack( pChunk->GetRef() );
		pChunk->SetParent( GetRef(), World );
		OnChildAdded( *pChunk, World );
	}
}

void TActorAsteroid::RecalcGravity(TWorld& World)
{
	//	get the gravity component
	auto* pGravity = GetComponent<TComGravity>();
	assert( pGravity );
	if ( !pGravity )
		return;

	//	get the accumulated shape
	ofShapeCircle2 Shape = GetTreeLocalCollisionShape(World);
	pGravity->SetLocalGravityShape( Shape );
}


bool TActorAsteroid::Update(float TimeStep,TWorld& World)
{
	//	no more parts, destroy
	if ( mChunks.IsEmpty() )
		return false;
	return true;
}


TActorAsteroidChunk::TActorAsteroidChunk(const ofShapePolygon2& Polygon,TWorld& World) :
	mHealth	( 1.f )
{
	auto& Collision = AddComponent<TComCollision>();
	Collision.mShape.SetPolygon( Polygon );
}


void TActorAsteroidChunk::OnImpact(const TAsteroidChunkImpactMeta& Impact)
{
	mHealth -= Impact.mChunkHealthReduction;
}
	
bool TActorAsteroidChunk::Update(float TimeStep,TWorld& World)
{
	if ( mHealth <= 0.f )
		return false;
	return true;
}





bool TActorAutoPath::Update(float TimeStep,TWorld& World)
{
	if ( !GetParent() )
		return true;

	//	grab parent's latest position...
	vec2f ParentPos = GetWorldPosition2();

	//	empty path? initialise it
	if ( mWorldPath.IsEmpty() )
		mWorldPath.PushBack( ParentPos );

	//	see if we need to add a new point
	auto& PrevPos = mWorldPath.GetBack();
	float DeltaLengthSq = (PrevPos - ParentPos).lengthSquared();
	if ( DeltaLengthSq > PATH_MIN_SEGMENT_LENGTH*PATH_MIN_SEGMENT_LENGTH )
		mWorldPath.PushBack( ParentPos );

	return true;
}

void TActorAutoPath::Render(float TimeStep,const TRenderSettings& RenderSettings)
{
	TRenderSceneScope Scene(__FUNCTION__);
	ofSetColor( GetColour() );
	ofSetLineWidth( PATH_WIDTH );
	ofFill();

	//	rendering in world space
	for ( int i=0;	i<mWorldPath.GetSize()-1;	i++ )
	{
		vec3f Start( mWorldPath[i].x, mWorldPath[i].y, GetZ() );
		vec3f End( mWorldPath[i+1].x, mWorldPath[i+1].y, GetZ() );
		ofLine( Start, End );
	}
}



