#include "TStarWorld.h"
#include "TGame.h"





void TStarWorld::OnCollision(const TCollision& Collision,TGame& Game,TActorRocket& ActorA,TActorDeathStar& ActorB)
{
	//	if the rocket hits it's owner player, ignore the collision
	TPlayer* pPlayer = Game.GetPlayer( ActorB.GetRef() );
	assert( pPlayer );
	if ( pPlayer && pPlayer->mRef == ActorA.mPlayerRef )
		return;

	//	kablammo!
	TGamePacket_CollisionRocketPlayer Packet;
	Packet.mActorRocket = ActorA;
	Packet.mActorDeathStar = ActorB;
	Packet.mIntersection = Collision.mIntersection;
	Game.PushPacket( Packet );
}


void TStarWorld::OnCollision(const TCollision& Collision,TGame& Game,TActorRocket& ActorA,TActorSentry& ActorB)
{
	//	if the rocket hits it's owner player, ignore the collision
	TPlayer* pPlayer = Game.GetPlayer( ActorB.GetRef() );
	assert( pPlayer );
	if ( pPlayer && pPlayer->mRef == ActorA.mPlayerRef )
		return;

	//	kablammo!
	TGamePacket_CollisionRocketAndSentry Packet;
	Packet.mActorRocket = ActorA;
	Packet.mActorSentry = ActorB;
	Packet.mIntersection = Collision.mIntersection;
	Game.PushPacket( Packet );
}


void TStarWorld::OnCollision(const TCollision& Collision,TGame& Game,TActorRocket& ActorA,TActorAsteroidChunk& ActorB)
{
	//	blow up both!
	TGamePacket_CollisionRocketAndAsteroidChunk Packet;
	Packet.mActorRocket = ActorA;
	Packet.mActorAsteroidChunk = ActorB;
	Packet.mIntersection = Collision.mIntersection;
	Game.PushPacket( Packet );
}

bool TStarWorld::HandleCollision(const TCollision& Collision,TGame& Game,TActor& ActorA,TActor& ActorB)
{
	if ( TryCollision<TActorRocket,TActorDeathStar>( Collision, Game, ActorA, ActorB ) )			return true;
	if ( TryCollision<TActorRocket,TActorSentryRocket>( Collision, Game, ActorA, ActorB ) )		return true;
	if ( TryCollision<TActorRocket,TActorSentryLaserBeam>( Collision, Game, ActorA, ActorB ) )	return true;
	if ( TryCollision<TActorRocket,TActorSentryRotation>( Collision, Game, ActorA, ActorB ) )	return true;
	if ( TryCollision<TActorRocket,TActorAsteroidChunk>( Collision, Game, ActorA, ActorB ) )		return true;

	//	unhandled
	return false;
}