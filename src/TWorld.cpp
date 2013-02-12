#include "TWorld.h"


void TWorld::Render(float TimeStep,const TRenderSettings& RenderSettings)
{
	//	render actors
	for ( int a=0;	a<mActors.GetSize();	a++ )
	{
		auto& Actor = *mActors[a];

		if ( RenderSettings.mMode == TRenderMode::Collision )
			Actor.RenderCollision( RenderSettings );
		else
			Actor.Render( TimeStep, RenderSettings );
	}
}



void TWorld::Update(float TimeStep)
{
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
			CollisionTests.PushBack( TCollisionTest( CollisionActors[a], CollisionActors[b] ) );

	//	execute tests
	for ( int t=CollisionTests.GetSize()-1;	t>=0;	t-- )
	{
		//	check for collision
		auto& ColA = *CollisionTests[t].mActorA;
		auto& ColB = *CollisionTests[t].mActorB;

		TIntersection Intersection;
		if ( !SoyPhysics::GetIntersection( ColA.mShape.mCircle, ColB.mShape.mCircle, Intersection ) )
		{
			CollisionTests.RemoveBlock( t, 1 );
			continue;
		}

		//	handle & react to collision
		mCollisions.PushBack( TCollision( CollisionTests[t], Intersection ) );
	}
}


void TWorld::DestroyActor(TActor& Actor)
{
	TActor* pActor = &Actor;

	//	remove from lists
	mActors.Remove( pActor );

	//	find in collisions (this will be redundant when we omit pointers from the TActorRef)
	for ( int i=mCollisions.GetSize()-1;	i>=0;	i-- )
	{
		auto& Collision = mCollisions[i];
		if ( Collision.mActorA.mActor != pActor && Collision.mActorB.mActor != pActor )
			continue;

		mCollisions.RemoveBlock( i, 1 );
	}

	//	delete
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
