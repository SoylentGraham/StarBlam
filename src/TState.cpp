#include "TState.h"
#include "TGame.h"
#include "TApp.h"



TGame*& TState::GetGame()
{
	return mApp.mGame;
}


TStateGame::TStateGame(TApp& App) :
	TState	( App )
{
	//	create & init game
	TGameMeta GameMeta;
	GameMeta.mNetworkMode = TGameNetworkMode::LocalSwap;
	GameMeta.mPlayerA = TPlayerMeta( "Emmett", ofColour(209,174,52) );
	GameMeta.mPlayerB = TPlayerMeta( "Biff", ofColour(226, 37, 37) );
	
	auto& pGame = GetGame();
	pGame = new TGame( GameMeta );
	pGame->Init();
}


TStateGame::~TStateGame()
{
}

	 
void TStateGame::Update(float TimeStep)
{
	auto& pGame = GetGame();
	if ( !pGame )
		return;
	
	//	gr: todo: pull out UI input gestures first and deliver what's left to the game if applicable
	pGame->UpdateInput( mApp.mInput );

	//	update logic
	pGame->Update( TimeStep );

}

	 
void TStateGame::Render(float TimeStep)
{
	auto& pGame = GetGame();
	if ( !pGame )
		return;

	pGame->Render( TimeStep );
}


