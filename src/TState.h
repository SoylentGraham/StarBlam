#pragma once

#include "Main.h"
class TApp;
class TGame;


class TState : public Soy::TState<TApp>
{
public:
	TState(TApp& App) :
		Soy::TState<TApp>	( App ),
		mApp				( App )
	{
	}

	TGame*&			GetGame();

public:
	TApp&	mApp;
};




class TStateGame : public TState
{
public:
	TStateGame(TApp& App);
	virtual ~TStateGame();

	virtual void	Update(float TimeStep);	//	update
	virtual void	Render(float TimeStep);	//	render

};


