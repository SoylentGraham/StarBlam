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
		EndLaserBeam,			//	wait for beam to finish before going to next player
		Collision_ProjectileAndPlayer,
		Collision_ProjectileAndSentry,
		Collision_ProjectileAndAsteroidChunk,
		ChangeTurn,
		StartDrag,
		UpdateDrag,
		EndDrag,
	};

	BufferString<100>	ToString(TGamePackets::Type Enum);
	void				GetArray(ArrayBridge<TGamePackets::Type>& Array);
};
SOY_DECLARE_ENUM( TGamePackets );


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




class TGamePacket_FireRocket : public TGamePacketDerivitive<TGamePackets::FireRocket>
{
public:
	ofLine2	mFiringLine;
	TRef	mPlayerRef;
};


class TGamePacket_FireMissile : public TGamePacketDerivitive<TGamePackets::FireMissile>
{
public:
	TRef			mPlayerRef;
	ofShapePath2	mFiringPath;
};

template<> bool Soy::ToRawData(Array<char>& Data,const TGamePacket_FireMissile& Packet);
template<> bool Soy::FromRawData(TGamePacket_FireMissile& Packet,const Array<char>& Data);


class TGamePacket_EndLaserBeam : public TGamePacketDerivitive<TGamePackets::EndLaserBeam>
{
public:
	TActorRef	mSentry;
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


class TGamePacket_ChangeTurn :  public TGamePacketDerivitive<TGamePackets::ChangeTurn>
{
public:
	TRef			mNextPlayer;
};

class TGamePacket_StartDrag :  public TGamePacketDerivitive<TGamePackets::StartDrag>
{
public:
	TRef			mPlayer;
	TActorRef		mSentry;
	vec2f			mScreenPos;
};

class TGamePacket_EndDrag :  public TGamePacketDerivitive<TGamePackets::EndDrag>
{
public:
	//	params for verification
	TRef			mCurrentDragPlayer;
};

class TGamePacket_UpdateDrag :  public TGamePacketDerivitive<TGamePackets::UpdateDrag>
{
public:
	TRef			mCurrentDragPlayer;
	vec2f			mScreenPos;
};

