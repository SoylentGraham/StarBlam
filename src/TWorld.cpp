#include "TWorld.h"
#include "TGame.h"


void TWorld::Render(float TimeStep,const TRenderSettings& RenderSettings)
{
	RealDestroy();

	//	render actors
	for ( int a=0;	a<mActors.GetSize();	a++ )
	{
		auto& Actor = *mActors[a];
		/*
		if ( RenderSettings.mMode == TRenderMode::Collision )
			Actor.RenderCollision( RenderSettings );
		else
		*/
			Actor.Render( TimeStep, RenderSettings );
	}
}



void TWorld::Update(float TimeStep)
{
	RealDestroy();

	Array<TCollisionActor> CollisionActors;

	//	update actors (physics etc)
	for ( int a=mActors.GetSize()-1;	a>=0;	a-- )
	{
		auto& Actor = *mActors[a];
		if ( !Actor.Update( TimeStep, *this ) )
		{
			DestroyActor( Actor );
			continue;
		}

		TCollisionActor ColActor( Actor );
		if ( ColActor.IsValid() )
			CollisionActors.PushBack( ColActor );
	}

	DoCollisions( CollisionActors );
}





void TWorld::DoCollisions(Array<TCollisionActor>& CollisionActors)
{
	//	make up test collisions
	Array<TCollisionTest> CollisionTests;
	for ( int a=0;	a<CollisionActors.GetSize();	a++ )
		for ( int b=a+1;	b<CollisionActors.GetSize();	b++ )
			if ( CanCollide( *CollisionActors[a].mActor, *CollisionActors[b].mActor ) )
				CollisionTests.PushBack( TCollisionTest( CollisionActors[a], CollisionActors[b] ) );

	//	execute tests
	for ( int t=CollisionTests.GetSize()-1;	t>=0;	t-- )
	{
		//	check for collision
		auto& ColA = *CollisionTests[t].mActorA;
		auto& ColB = *CollisionTests[t].mActorB;

		TIntersection Intersection;
		if ( !SoyPhysics::GetIntersection( ColA.mShape, ColB.mShape, Intersection ) )
		{
			CollisionTests.RemoveBlock( t, 1 );
			continue;
		}

		//	handle & react to collision
		mCollisions.PushBack( TCollision( CollisionTests[t], Intersection ) );
	}
}



void TWorld::Disconnect(TActor& Actor)
{
	//	remove all parent/child links and add to destroy list
	mDestroyList.PushBackUnique( Actor.GetRef() );	

	//	remove from parent
	TActor* pParent = GetActor( Actor.GetParent() );
	if ( pParent )
	{
		pParent->OnChildDestroyed( Actor.GetRef() );
		Actor.SetParent( TActorRef() );
	}

	//	disconnect children
	auto Children = Actor.GetChildren();
	for ( int i=0;	i<Children.GetSize();	i++ )
	{
		TActorRef Child = Children[i];
		TActor* pChild = GetActor( Child );
		if ( pChild )
			Disconnect( *pChild );
	}

}


void TWorld::DestroyActor(TActor& Actor)
{
	Disconnect( Actor );
}


void TWorld::RealDestroy()
{
	while ( !mDestroyList.IsEmpty() )
	{
		TActorRef Ref = mDestroyList.PopAt(0);

		//	grab actor (will fail if has been destroyed in the meantime)
		TActor* pActor = GetActor( Ref );
		if ( !pActor )
			continue;

		//	do real destruction
		RealDestroyActor( *pActor );
	}
}


void TWorld::RealDestroyActor(TActor& Actor)
{
	TActor* pActor = &Actor;

	//	remove from internal lists
	mActors.Remove( pActor );

	//	find in collision cache (this will be redundant when we omit pointers from the TActorRef)
	for ( int i=mCollisions.GetSize()-1;	i>=0;	i-- )
	{
		auto& Collision = mCollisions[i];
		if ( Collision.mActorA.mActor != pActor && Collision.mActorB.mActor != pActor )
			continue;

		mCollisions.RemoveBlock( i, 1 );
	}

	//	finally, delete
	delete pActor;
}



TCollision TWorld::PopCollision()
{
	if ( mCollisions.IsEmpty() )
		return TCollision();

	TCollision Collision = mCollisions[0];
	mCollisions.RemoveBlock( 0, 1 );

	return Collision;
}


bool TWorld::DoRaycast(const TRaycast& Raycast,TRaycastResult& Result)
{
	//	ray cast against every actor with a collisionshape
	return false;
}



TActor* TWorld::GetActor(const TActorRef& Actor)
{
	//	check the actor still exists
	TActor* pActor = const_cast<TActor*>( Actor.GetActor() );
	if ( !mActors.Find( pActor ) )
		return NULL;

	return pActor;
}

void TWorld::DestroyActor(const TActorRef ActorRef)
{
	auto* pActor = GetActor( ActorRef );
	if ( !pActor )
		return;

	DestroyActor( *pActor );
}



void TWorld::UpdateCollisions(TGame& Game)
{
	while ( true )
	{
		TCollision Collision = PopCollision();
		if ( !Collision.IsValid() )
			break;

		//	grab the actors for the functions
		TActor* pActorA = GetActor( Collision.mActorA );
		TActor* pActorB = GetActor( Collision.mActorB );
		if ( !pActorA || !pActorB )
			continue;
		auto& ActorA = *pActorA;
		auto& ActorB = *pActorB;

		//	high level collision handling
		HandleCollision( Collision, Game, ActorA, ActorB );
	}
}


