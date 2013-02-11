#pragma once

#include "Main.h"


namespace TGamePackets
{
	enum Type
	{
		FireRocket
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



class TGamePacket_FireRocket : public TGamePacketDerivitive<TGamePackets::FireRocket>
{
public:
	ofShapeLine2	mFiringLine;
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



//	clone this packet and push it
template<class PACKET>
void TGamePacketContainer::PushPacket(const PACKET& Packet)
{
	TGamePacket* pPacketCopy = new PACKET( Packet );
	PushPacket( pPacketCopy );
}


