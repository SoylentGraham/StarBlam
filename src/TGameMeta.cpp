#include "TGameMeta.h"
#include "TGame.h"


namespace TLGame
{
	TRef AllocPlayerRef()
	{
		static TRef LastRef( 0 );
		LastRef.mRef++;
		return LastRef;
	}
}


TPlayerMeta::TPlayerMeta(const TString& Name,const ofColour& Colour) :
	mName	( Name ),
	mColour	( Colour ),
	mRef	( TLGame::AllocPlayerRef() )
{
}

TPlayerMeta::TPlayerMeta() :
	mName	( "X" ),
	mColour	( ofColour( 255, 255, 0 ) ),
	mRef	( TLGame::AllocPlayerRef() )
{
}
