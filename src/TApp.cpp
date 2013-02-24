#include "TApp.h"
#include "TStateFrontEnd.h"


TApp::TApp() :
	mGame	( NULL ),
	mState	( NULL )
{
}


void TApp::setup()
{
	//	sys init
	TRender::InitFont();

	//	init state
	//ChangeState<TStateGame>();
	ChangeState<TStatePickMode>();
}


void TApp::update()
{
	//	work this out!
	float TimeStep = 1.f / ofGetFrameRate();

	//	exit old states
	for ( int i=mExitingState.GetSize()-1;	i>=0;	i-- )
	{
		auto* pState = mExitingState[i];
		if ( !pState->Exit() )
			continue;

		delete pState;
		mExitingState.RemoveBlock( i, 1 );
	}

	//	err should be in a state
	if ( !mState )
	{
		assert( mState );
		ChangeState<TStateGame>();
	}

	//	update current state
	if ( mState )
	{
		mState->Update( TimeStep );
	}

}

void TApp::draw()
{
	//	render game
	float TimeStep = 1.f / ofGetFrameRate();

	//	render game background
	TRenderSceneScope Scene(__FUNCTION__);
	ofClear( ofColour(104,56,55) );

	//	render old states
	for ( int i=mExitingState.GetSize()-1;	i>=0;	i-- )
	{
		auto* pState = mExitingState[i];
		TRenderSceneScope Scene(__FUNCTION__);
		pState->Render( TimeStep );
	}

	//	update
	if ( mState )
	{
		TRenderSceneScope Scene(__FUNCTION__);
		mState->Render( TimeStep );
	}
}

void TApp::exit()
{
	//	kill states...
	
}
