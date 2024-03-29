#pragma once

#include "Main.h"
#include "TActor.h"
#include "TPhysics.h"

class TGame;


class TRaycast
{
public:
	TRaycast(const ofShapeCapsule2& Capsule) :
		mCapsule	( Capsule )
	{
	}
	TRaycast(const vec2f& Pos,const vec2f& Normal) :
		mCapsule	( ofLine2( Pos, Pos+Normal ), 0.01f )
	{
	}

	void				IgnoreCollisionWith(TActor* Actor,TWorld& World)	{	if ( Actor )	IgnoreCollisionWith( *Actor, World );	}
	void				IgnoreCollisionWith(TActor& Actor,TWorld& World);	//	ignore actor and it's children

public:
	ofShapeCapsule2		mCapsule;
	Array<TActorRef>	mIgnoreList;	//	ignore collision with these actors
};

class TRaycastResult
{
public:
	bool			IsValid() const	{	return mIntersection.IsValid();	}

public:
	TIntersection	mIntersection;
	TActorRef		mActor;			//	actor we hit
};

//----------------------------------------------
//	world simulator. Handles world interactions and tells the game what to do (though if passive then game will only react from the current player)
//	this is all low level apart from the virtual functions
//----------------------------------------------
class TWorld
{
public:
	void			Render(float TimeStep,const TRenderSettings& RenderSettings);
	void			Update(float TimeStep);

	void			UpdateCollisions(TGame& Game);		//	gather collisions

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

protected:	//	high level
	virtual bool		HandleCollision(const TCollision& Collision,TGame& Game,TActor& ActorA,TActor& ActorB)=0;
	virtual bool		CanCollide(const TActor& a,const TActor& b)=0;

private:
	void				DoCollisions();
	TCollision			PopCollision();
	
	void				Disconnect(TActor& Actor);			//	remove all parent/child links and add to destroy list
	void				RealDestroy();
	void				RealDestroyActor(TActor& Actor);

private:
	Array<TCollision>	mCollisions;
	Array<TActor*>		mActors;
	Array<TActorRef>	mDestroyList;
};


template<class ACTORTYPE>
ACTORTYPE* TWorld::CreateActor()
{
	ACTORTYPE* pActor = new ACTORTYPE( *this );
	if ( !pActor )
		return NULL;

	mActors.PushBack( pActor );
	return pActor;
}



template<class ACTORTYPE,typename P1>
ACTORTYPE* TWorld::CreateActor(P1& Param1)
{
	ACTORTYPE* pActor = new ACTORTYPE( Param1, *this );
	if ( !pActor )
		return NULL;

	mActors.PushBack( pActor );
	return pActor;
}



template<class ACTORTYPE,typename P1,typename P2>
ACTORTYPE* TWorld::CreateActor(P1& Param1,P2& Param2)
{
	ACTORTYPE* pActor = new ACTORTYPE( Param1, Param2, *this );
	if ( !pActor )
		return NULL;

	mActors.PushBack( pActor );
	return pActor;
}


