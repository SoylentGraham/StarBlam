#include "TGamePackets.h"




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
