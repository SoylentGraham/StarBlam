#pragma once

#include "TWorld.h"

class TStarWorld : public TWorld
{
protected:
	virtual bool	CanCollide(const TActor& a,const TActor& b);
	virtual bool	HandleCollision(const TCollision& Collision,TGame& Game,TActor& ActorA,TActor& ActorB);

private:
	template<class ACTORA,class ACTORB>
	bool			TryCollision(const TCollision& Collision,TGame& Game,TActor& ActorA,TActor& ActorB);

	void			OnCollision(const TCollision& Collision,TGame& Game,TActor& ActorA,TActor& ActorB)	{}	//	fall through for cases we dont really handle
	void			OnCollision(const TCollision& Collision,TGame& Game,TActorProjectile& ActorA,TActorDeathStar& ActorB);
	void			OnCollision(const TCollision& Collision,TGame& Game,TActorProjectile& ActorA,TActorSentry& ActorB);
	void			OnCollision(const TCollision& Collision,TGame& Game,TActorProjectile& ActorA,TActorAsteroidChunk& ActorB);
};



template<class ACTORA,class ACTORB>
bool TStarWorld::TryCollision(const TCollision& Collision,TGame& Game,TActor& ActorA,TActor& ActorB)
{
	if ( ActorA == ACTORA::TYPE && ActorB == ACTORB::TYPE )
	{
		OnCollision( Collision, Game, dynamic_cast<ACTORA&>( ActorA ), dynamic_cast<ACTORB&>( ActorB ) );
		return true;
	}

	//	try swapped types (note OnCollision swaps too, cast vs name)
	if ( ActorA == ACTORB::TYPE && ActorB == ACTORA::TYPE )
	{
		OnCollision( Collision, Game, dynamic_cast<ACTORA&>( ActorB ), dynamic_cast<ACTORB&>( ActorA ) );
		return true;
	}
	return false;
}
