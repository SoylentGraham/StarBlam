#pragma once

#include "Main.h"
#include "TActor.h"
#include "TPhysics.h"



class TRaycast
{
public:
	TRaycast(const vec2f& Pos,const vec2f& Normal) :
		mPosition	( Pos ),
		mNormal		( Normal )
	{
	}

public:
	vec2f	mPosition;
	vec2f	mNormal;
};

class TRaycastResult
{
public:
	vec2f		mIntersection;
	TActorRef	mActor;			//	first actor we hit
};


class TWorld
{
public:
	void			Render(float TimeStep,const TRenderSettings& RenderSettings);
	void			Update(float TimeStep);

	void			DoCollisions(Array<TCollisionActor>& CollisionActors);
	TCollision		PopCollision();

	bool			DoRaycast(const TRaycast& Raycast,TRaycastResult& Result);


	template<class ACTORTYPE>				
	ACTORTYPE*		CreateActor();
	template<class ACTORTYPE,typename P1>	
	ACTORTYPE*		CreateActor(P1& Param1);
	template<class ACTORTYPE,typename P1,typename P2>	
	ACTORTYPE*		CreateActor(P1& Param1,P2& Param2);
	void			DestroyActor(TActor& Actor);
	void			DestroyActor(const TActorRef ActorRef);
	TActor*			GetActor(const TActorRef& Actor);
	template<class ACTORTYPE>				
	ACTORTYPE*		GetActor(const TActorRef& Actor)		{	return static_cast<ACTORTYPE*>( GetActor(Actor) );	}

private:
	void				Disconnect(TActor& Actor);			//	remove all parent/child links and add to destroy list
	void				RealDestroy();
	void				RealDestroyActor(TActor& Actor);

public:
	Array<TCollision>	mCollisions;
	Array<TActor*>		mActors;
	Array<TActorRef>	mDestroyList;
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
ACTORTYPE* TWorld::CreateActor(P1& Param1)
{
	ACTORTYPE* pActor = new ACTORTYPE( Param1 );
	if ( !pActor )
		return NULL;

	mActors.PushBack( pActor );
	return pActor;
}



template<class ACTORTYPE,typename P1,typename P2>
ACTORTYPE* TWorld::CreateActor(P1& Param1,P2& Param2)
{
	ACTORTYPE* pActor = new ACTORTYPE( Param1, Param2 );
	if ( !pActor )
		return NULL;

	mActors.PushBack( pActor );
	return pActor;
}


