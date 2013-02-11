#include "testApp.h"


testApp::testApp() :
	mGame	( NULL )
{
}


void testApp::setup()
{
	exit();

	//	create new game
	TPlayerMeta Player1("Emmett", ofColour(209,174,52) );
	TPlayerMeta Player2("Biff", ofColour(226, 37, 37) );
	mGame = new TGame( Player1, Player2 );
	mGame->Init();
}

void testApp::update()
{
	//	work this out!
	float TimeStep = 1.f / ofGetFrameRate();

	//	update game
	if ( mGame )
	{
		//	gr: todo: pull out UI input gestures first and deliver what's left to the game if applicable
		mGame->UpdateInput( mInput );

		//	update logic
		mGame->Update( TimeStep );
	}

	//	app debug
}

void testApp::draw()
{
	//	render game
	float TimeStep = 1.f / ofGetFrameRate();

	if ( mGame )
	{
		mGame->Render( TimeStep );
	}

	//	render App debug
}

void testApp::exit()
{
	if ( mGame )
	{
		delete mGame;
		mGame = NULL;
	}
}
