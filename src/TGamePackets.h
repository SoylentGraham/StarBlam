#pragma once

#include "Main.h"
#include "TActor.h"
#include "TPhysics.h"


namespace TGamePackets
{
	enum Type
	{
		FireRocket,
		FireMissile,
		Collision_ProjectileAndPlayer,
		Collision_ProjectileAndSentry,
		Collision_ProjectileAndAsteroidChunk,
	};
};


class TGamePacket : public SoyPacket<TGamePackets::Type>
{
public:
};


template<TGamePackets::Type PACKETTYPE>
class TGamePacketDerivitive : public TGamePacket
{
public:
	static const TGamePackets::Type TYPE = PACKETTYPE;

public:
	virtual TGamePackets::Type	GetType() const	{	return PACKETTYPE;	}
};



class TGamePacketContainer : public SoyPacketContainer<TGamePackets::Type PACKETTYPE>
{
public:
};



class TGamePacket_FireRocket : public TGamePacketDerivitive<TGamePackets::FireRocket>
{
public:
	ofLine2	mFiringLine;
	TRef	mPlayerRef;
};


class TGamePacket_FireMissile : public TGamePacketDerivitive<TGamePackets::FireMissile>
{
public:
	ofShapePath2	mFiringPath;
	TRef			mPlayerRef;
};


class TGamePacket_CollisionProjectileAndPlayer : public TGamePacketDerivitive<TGamePackets::Collision_ProjectileAndPlayer>
{
public:
	TActorRef		mActorProjectile;
	TActorRef		mActorDeathStar;
	TIntersection	mIntersection;
};


class TGamePacket_CollisionProjectileAndSentry : public TGamePacketDerivitive<TGamePackets::Collision_ProjectileAndSentry>
{
public:
	TActorRef		mActorProjectile;
	TActorRef		mActorSentry;
	TIntersection	mIntersection;
};


class TGamePacket_CollisionProjectileAndAsteroidChunk : public TGamePacketDerivitive<TGamePackets::Collision_ProjectileAndAsteroidChunk>
{
public:
	TActorRef		mActorProjectile;
	TActorRef		mActorAsteroidChunk;
	TIntersection	mIntersection;
	TAsteroidChunkImpactMeta	mChunkImpactMeta;
};





//	clone this packet and push it
template<class PACKET>
void TGamePacketContainer::PushPacket(const PACKET& Packet)
{
	TGamePacket* pPacketCopy = new PACKET( Packet );
	PushPacket( pPacketCopy );
}


