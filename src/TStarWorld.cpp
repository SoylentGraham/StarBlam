#include "TStarWorld.h"
#include "TGame.h"





void TStarWorld::OnCollision(const TCollision& Collision,TGame& Game,TActorProjectile& ActorA,TActorDeathStar& ActorB)
{
	//	if the rocket hits it's owner player, ignore the collision
	TPlayer* pPlayer = Game.GetPlayer( ActorB.GetRef() );
	assert( pPlayer );
	if ( pPlayer && pPlayer->mRef == ActorA.GetOwnerPlayer() )
		return;

	//	kablammo!
	TGamePacket_CollisionProjectileAndPlayer Packet;
	Packet.mActorProjectile = ActorA;
	Packet.mActorDeathStar = ActorB;
	Packet.mIntersection = Collision.mIntersection;
	Game.PushPacket( Packet );
}


void TStarWorld::OnCollision(const TCollision& Collision,TGame& Game,TActorProjectile& ActorA,TActorSentry& ActorB)
{
	//	if the rocket hits it's owner player, ignore the collision
	TPlayer* pPlayer = Game.GetPlayer( ActorB.GetRef() );
	assert( pPlayer );
	if ( pPlayer && pPlayer->mRef == ActorA.GetOwnerPlayer() )
		return;

	//	kablammo!
	TGamePacket_CollisionProjectileAndSentry Packet;
	Packet.mActorProjectile = ActorA;
	Packet.mActorSentry = ActorB;
	Packet.mIntersection = Collision.mIntersection;
	Game.PushPacket( Packet );
}


void TStarWorld::OnCollision(const TCollision& Collision,TGame& Game,TActorProjectile& ActorA,TActorAsteroidChunk& ActorB)
{
	//	blow up both!
	TGamePacket_CollisionProjectileAndAsteroidChunk Packet;
	Packet.mActorProjectile = ActorA;
	Packet.mActorAsteroidChunk = ActorB;
	Packet.mIntersection = Collision.mIntersection;
	Game.PushPacket( Packet );
}

bool TStarWorld::HandleCollision(const TCollision& Collision,TGame& Game,TActor& ActorA,TActor& ActorB)
{
	if ( TryCollision<TActorRocket,TActorDeathStar>( Collision, Game, ActorA, ActorB ) )		return true;
	if ( TryCollision<TActorRocket,TActorSentryRocket>( Collision, Game, ActorA, ActorB ) )		return true;
	if ( TryCollision<TActorRocket,TActorSentryMissile>( Collision, Game, ActorA, ActorB ) )	return true;
	if ( TryCollision<TActorRocket,TActorSentryLaserBeam>( Collision, Game, ActorA, ActorB ) )	return true;
	if ( TryCollision<TActorRocket,TActorSentryRotation>( Collision, Game, ActorA, ActorB ) )	return true;
	if ( TryCollision<TActorRocket,TActorAsteroidChunk>( Collision, Game, ActorA, ActorB ) )	return true;
	if ( TryCollision<TActorMissile,TActorDeathStar>( Collision, Game, ActorA, ActorB ) )		return true;
	if ( TryCollision<TActorMissile,TActorSentryRocket>( Collision, Game, ActorA, ActorB ) )	return true;
	if ( TryCollision<TActorMissile,TActorSentryMissile>( Collision, Game, ActorA, ActorB ) )	return true;
	if ( TryCollision<TActorMissile,TActorSentryLaserBeam>( Collision, Game, ActorA, ActorB ) )	return true;
	if ( TryCollision<TActorMissile,TActorSentryRotation>( Collision, Game, ActorA, ActorB ) )	return true;
	if ( TryCollision<TActorMissile,TActorAsteroidChunk>( Collision, Game, ActorA, ActorB ) )	return true;

	//	unhandled
	return false;
}

bool TStarWorld::CanCollide(const TActor& a,const TActor& b)
{
	bool aIsAsteroid = ( a.GetType() == TActors::Asteroid || a.GetType() == TActors::AsteroidChunk );
	bool bIsAsteroid = ( b.GetType() == TActors::Asteroid || b.GetType() == TActors::AsteroidChunk );
	if ( aIsAsteroid && bIsAsteroid )
		return false;

	return true;
}
