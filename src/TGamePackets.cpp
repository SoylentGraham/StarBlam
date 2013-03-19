#include "TGamePackets.h"


BufferString<100> TGamePackets::ToString(TGamePackets::Type Enum)
{
	switch ( Enum )
	{
		case FireRocket:		return "FireRocket";
		case FireMissile:		return "FireMissile";
		case EndLaserBeam:		return "EndLaserBeam";
		case Collision_ProjectileAndPlayer:		return "Collision_ProjectileAndPlayer";
		case Collision_ProjectileAndSentry:		return "Collision_ProjectileAndSentry";
		case Collision_ProjectileAndAsteroidChunk:		return "Collision_ProjectileAndAsteroidChunk";
		case ChangeTurn:		return "ChangeTurn";
		case StartDrag:			return "StartDrag";
		case UpdateDrag:		return "UpdateDrag";
		case EndDrag:			return "EndDrag";
	};

	assert(false);
	return "???";
}

void TGamePackets::GetArray(ArrayBridge<TGamePackets::Type>& Array)
{
	Array.PushBack( FireRocket );
	Array.PushBack( FireMissile );
	Array.PushBack( Collision_ProjectileAndPlayer );
	Array.PushBack( Collision_ProjectileAndSentry );
	Array.PushBack( Collision_ProjectileAndAsteroidChunk );
	Array.PushBack( ChangeTurn );
	Array.PushBack( StartDrag );
	Array.PushBack( UpdateDrag );
	Array.PushBack( EndDrag );
}



template<>
bool Soy::ToRawData(Array<char>& Data,const TGamePacket_FireMissile& Packet)
{
	TArrayWriter Writer( Data );

	Writer.Write( Packet.mPlayerRef );
	Writer.WriteArray( Packet.mFiringPath.mPath );

	return true;
}




template<>
bool Soy::FromRawData(TGamePacket_FireMissile& Packet,const Array<char>& Data)
{
	TArrayReader Reader( Data );
	if ( !Reader.Read( Packet.mPlayerRef ) )
		return false;
	if ( !Reader.ReadArray( Packet.mFiringPath.mPath ) )
		return false;
	return true;
}
