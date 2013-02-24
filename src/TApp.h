#pragma once

#include "Main.h"
#include "TGame.h"
#include "TState.h"


class TApp : public SoyApp
{
public:
	TApp();

	virtual void setup();
	virtual void update();
	virtual void draw();
	virtual void exit();

	template<class TSTATE>
	void			ChangeState();

public:
	SoyNet::TManager	mNetManager;
	TGame*				mGame;
	TState*				mState;
	Array<TState*>		mExitingState;	//	currently destructing states
};





template<class TSTATE>
void TApp::ChangeState()
{
	//	exit the current state
	if ( mState )
	{
		mExitingState.PushBackUnique( mState );
		mState = NULL;
	}

	//	create new one
	mState = new TSTATE( *this );
}

