#pragma once

#include "TWorld.h"

class TStarWorld : public TWorld
{
public:
	virtual bool	HandleCollision(const TCollision& Collision,TGame& Game,TActor& ActorA,TActor& ActorB);
	template<class ACTORA,class ACTORB>
	bool			TryCollision(const TCollision& Collision,TGame& Game,TActor& ActorA,TActor& ActorB);

	void			OnCollision(const TCollision& Collision,TGame& Game,TActor& ActorA,TActor& ActorB)	{}	//	fall through for cases we dont really handle

	void			OnCollision(const TCollision& Collision,TGame& Game,TActorRocket& ActorA,TActorDeathStar& ActorB);
	void			OnCollision(const TCollision& Collision,TGame& Game,TActorRocket& ActorA,TActorSentry& ActorB);
	void			OnCollision(const TCollision& Collision,TGame& Game,TActorRocket& ActorA,TActorAsteroidChunk& ActorB);
};



template<class ACTORA,class ACTORB>
bool TStarWorld::TryCollision(const TCollision& Collision,TGame& Game,TActor& ActorA,TActor& ActorB)
{
	if ( ActorA == ACTORA::TYPE && ActorB == ACTORB::TYPE )
	{
		OnCollision( Collision, Game, static_cast<ACTORA&>( ActorA ), static_cast<ACTORB&>( ActorB ) );
		return true;
	}
	if ( ActorA == ACTORB::TYPE && ActorA == ACTORB::TYPE )
	{
		OnCollision( Collision, Game, static_cast<ACTORB&>( ActorB ), static_cast<ACTORA&>( ActorA ) );
		return true;
	}
	return false;
}
