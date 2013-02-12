#pragma once

#include "Main.h"
#include "TActor.h"
#include "TPhysics.h"


namespace TGamePackets
{
	enum Type
	{
		FireRocket,
		Collision_RocketPlayer,
	};
};


class TGamePacket
{
public:
	virtual TGamePackets::Type	GetType() const=0;
};


template<TGamePackets::Type PACKETTYPE>
class TGamePacketDerivitive : public TGamePacket
{
public:
	virtual TGamePackets::Type	GetType() const	{	return PACKETTYPE;	}
};



class TGamePacketContainer : public ofMutex
{
public:

	//	clone this packet and push it
	template<class PACKET>
	void					PushPacket(const PACKET& Packet);
	void					PushPacket(TGamePacket* pPacket);
	TGamePacket*			PopPacket();

protected:
	Array<TGamePacket*>		mPackets;
};



class TGamePacket_FireRocket : public TGamePacketDerivitive<TGamePackets::FireRocket>
{
public:
	ofShapeLine2	mFiringLine;
	TRef			mPlayerRef;
};


class TGamePacket_CollisionRocketPlayer : public TGamePacketDerivitive<TGamePackets::Collision_RocketPlayer>
{
public:
	TActorRef		mActorRocket;
	TActorRef		mActorDeathStar;
	TIntersection	mIntersection;
};





//	clone this packet and push it
template<class PACKET>
void TGamePacketContainer::PushPacket(const PACKET& Packet)
{
	TGamePacket* pPacketCopy = new PACKET( Packet );
	PushPacket( pPacketCopy );
}

