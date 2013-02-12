#pragma once

#include "Main.h"
#include "TActor.h"
#include "TPhysics.h"


class TWorld
{
public:
	void			Render(float TimeStep,const TRenderSettings& RenderSettings);
	void			Update(float TimeStep);

	void			DoCollisions(Array<TCollisionActor>& CollisionActors);
	TCollision		PopCollision();

	template<class ACTORTYPE>				
	ACTORTYPE*		CreateActor();
	template<class ACTORTYPE,typename P1>	
	ACTORTYPE*		CreateActor(const P1& Param1);
	template<class ACTORTYPE,typename P1,typename P2>	
	ACTORTYPE*		CreateActor(const P1& Param1,const P2& Param2);
	void			DestroyActor(TActor& Actor);
	void			DestroyActor(const TActorRef ActorRef)	{	DestroyActor( GetActor( ActorRef ) );	}
	TActor&			GetActor(const TActorRef& Actor)		{	return const_cast<TActor&>( *Actor.mActor );	}

public:
	Array<TCollision>	mCollisions;
	Array<TActor*>		mActors;
};




template<class ACTORTYPE>
ACTORTYPE* TWorld::CreateActor()
{
	ACTORTYPE* pActor = new ACTORTYPE;
	if ( !pActor )
		return NULL;

	mActors.PushBack( pActor );
	return pActor;
}



template<class ACTORTYPE,typename P1>
ACTORTYPE* TWorld::CreateActor(const P1& Param1)
{
	ACTORTYPE* pActor = new ACTORTYPE( Param1 );
	if ( !pActor )
		return NULL;

	mActors.PushBack( pActor );
	return pActor;
}



template<class ACTORTYPE,typename P1,typename P2>
ACTORTYPE* TWorld::CreateActor(const P1& Param1,const P2& Param2)
{
	ACTORTYPE* pActor = new ACTORTYPE( Param1, Param2 );
	if ( !pActor )
		return NULL;

	mActors.PushBack( pActor );
	return pActor;
}


