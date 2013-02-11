#pragma once

#include "Main.h"


namespace TRenderMode
{
	enum Type
	{
		Colour,		//	normal render
		//Depth,
		Collision,	//	show collision "layer"
	};
};


//	scene render settings
class TRenderSettings
{
public:
	TRenderMode::Type	mMode;
};


//	push/pop style for immediate rendering
class TRenderSceneScope
{
public:
	TRenderSceneScope(const char* SceneName);
	~TRenderSceneScope();
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
	ofColour	mColour;
};


//	renderable object data (link assets and node)
class TRenderNode
{
public:
	vec3f			mPosition;
	TMaterial		mMaterial;
};

