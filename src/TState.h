#pragma once

#include "Main.h"
class TApp;
class TGame;


class TState
{
public:
	TState(TApp& App);
	virtual ~TState()			{}

	virtual void	Update(float TimeStep)=0;	//	update
	virtual void	Render(float TimeStep)=0;	//	render
	virtual bool	Exit()						{	return true;	}	//	keep exiting until true (finished)

	TGame*&			GetGame();

protected:
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


