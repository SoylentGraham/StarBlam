#pragma once

#include "Main.h"
#include "TGame.h"


class testApp : public SoyApp
{
public:
	testApp();

	virtual void setup();
	virtual void update();
	virtual void draw();
	virtual void exit();

public:
	TGame*		mGame;
};
