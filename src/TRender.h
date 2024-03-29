#pragma once

#include "Main.h"
#include "TComponent.h"


namespace TRenderMode
{
	enum Type
	{
		Colour,		//	normal render
	};
};


//	scene render settings
class TRenderSettings
{
public:
	TRenderSettings() :
		mMode		( TRenderMode::Colour ),
		mComponent	( TComs::Invalid )
	{
	}

public:
	TRenderMode::Type	mMode;		//	
	TComs::Type			mComponent;	//	render a component rather than the actor
};




//	simple-2d-game-camera setup
class TCamera2
{
public:
	TCamera2() :
		mScroll	( 0,0 ),
		mZoom	( 0 )
	{
	}

	float		GetNearZ() const	{	return 0.1f;	}
	float		GetFarZ() const		{	return 1000.f;	}

public:
	vec2f		mScroll;
	float		mZoom;		//	normalised
};


//	basic shader data
class TMaterial
{
public:
	TMaterial(float z,ofColour Colour,bool Outline=false) :
		mZ			( z ),
		mColour		( Colour ),
		mOutline	( Outline )
	{
	}

public:
	bool		mOutline;
	ofColour	mColour;
	float		mZ;
};

