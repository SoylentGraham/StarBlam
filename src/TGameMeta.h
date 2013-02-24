#pragma once

#include "Main.h"


class TPlayerMeta
{
public:
	TPlayerMeta();
	TPlayerMeta(const TString& Name,const ofColour& Colour);

	inline bool	operator==(const TRef& Ref) const	{	return mRef == Ref;	}

public:
	ofColour	mColour;
	TString		mName;
	TRef		mRef;
};


namespace TGameNetworkMode
{
	enum Type
	{
		Invalid,	//	not setup
		LocalSwap,	//	both players local (swap ipad over)
	};
};

class TGameMeta
{
public:
	TGameMeta() :
		mNetworkMode	( TGameNetworkMode::Invalid )
	{
	}

public:
	TGameNetworkMode::Type		mNetworkMode;
	TPlayerMeta					mPlayerA;
	TPlayerMeta					mPlayerB;
};
