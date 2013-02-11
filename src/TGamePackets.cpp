#include "TGamePackets.h"






void TGamePacketContainer::PushPacket(TGamePacket* pPacket)
{
	ofMutex::ScopedLock Lock(*this);
	mPackets.PushBack( pPacket );
}


TGamePacket* TGamePacketContainer::PopPacket()
{
	ofMutex::ScopedLock Lock(*this);
	if ( mPackets.IsEmpty() )
		return NULL;

	TGamePacket* pPacket = mPackets[0];
	mPackets.RemoveBlock( 0, 1 );
	return pPacket;
}

