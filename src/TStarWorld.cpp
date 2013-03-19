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

bool IsTypePair(TActors::Type TypeA,TActors::Type TypeB,const TActor& a,const TActor& b)
{
	if ( a.GetType() == TypeA && b.GetType() == TypeB )
		return true;

	//	check reverse
	if ( a.GetType() == TypeB && b.GetType() == TypeA )
		return true;

	return false;
}

bool TStarWorld::CanCollide(const TActor& a,const TActor& b)
{
	if ( IsTypePair( TActors::Asteroid, TActors::AsteroidChunk, a, b ) )	return false;
	if ( IsTypePair( TActors::Asteroid, TActors::Asteroid, a, b ) )			return false;
	if ( IsTypePair( TActors::AsteroidChunk, TActors::AsteroidChunk, a, b ) )	return false;
	
	if ( IsTypePair( TActors::DeathStar, TActors::SentryLaserBeam, a, b ) )	return false;
	if ( IsTypePair( TActors::DeathStar, TActors::SentryMissile, a, b ) )	return false;
	if ( IsTypePair( TActors::DeathStar, TActors::SentryRocket, a, b ) )	return false;
	if ( IsTypePair( TActors::DeathStar, TActors::SentryRotation, a, b ) )	return false;

	if ( IsTypePair( TActors::LaserBeam, TActors::LaserBeam, a, b ) )	return false;

	return true;
}
