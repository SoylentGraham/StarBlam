#pragma once

#include "Main.h"
#include "TRender.h"

class TWorld;	//	cyclic dependancy!

#define WORLD_WIDTH	1000

#define ROCKET_Z	3
#define DEATHSTAR_Z	2
#define STARS_Z		1
#define GUI_Z		4
#define DRAG_Z		GUI_Z



class TCollisionShape
{
public:
	virtual bool		IsValid() const		{	return mCircle.mRadius > 0.f;	}
	virtual vec2f		GetCenter() const	{	return mCircle.mPosition;	}

public:
	ofShapeCircle2		mCircle;
};


class TActor
{
public:
	TActor(float z) :
		mPosition	( 0, 0, z )
	{
	}

	virtual void			Render(float TimeStep,const TRenderSettings& RenderSettings);
	virtual void			RenderCollision(const TRenderSettings& RenderSettings);
	virtual void			Update(float TimeStep,TWorld& World);
	
	vec3f					GetPosition3() const 			{	return mPosition;	}
	vec2f					GetPosition2() const 			{	return vec2f( GetPosition3().x, GetPosition3().y );	}
	void					SetPosition(const vec2f& Pos)	{	mPosition.x = Pos.x;	mPosition.y = Pos.y;	}
	virtual ofColour		GetColour() const				{	return ofColour(255,255,0);	}
	virtual TCollisionShape	GetCollisionShape() const		{	return TCollisionShape();	}

public:
	vec3f					mPosition;
};


class TActorDeathStar : public TActor
{
public:
	TActorDeathStar() :
		TActor	( DEATHSTAR_Z )
	{
	}

	virtual ofColour		GetColour() const			{	return mMaterial.mColour;	}
	virtual TCollisionShape	GetCollisionShape() const;

public:
	TMaterial			mMaterial;
};


class TActorStars : public TActor
{
public:
	TActorStars();

	virtual void		Render(float TimeStep,const TRenderSettings& RenderSettings);

	virtual ofColour	GetColour() const			{	return ofColour( 255,255,255 );	}

public:
	Array<vec3f>		mStars;
};


class TActorDrag : public TActor
{
public:
	TActorDrag();

	virtual void		Render(float TimeStep,const TRenderSettings& RenderSettings);

	virtual ofColour	GetColour() const			{	return ofColour( 230,10,10 );	}
	void				SetLine(const ofShapeLine2& Line);

public:
	ofShapeLine2		mLine;
};

class TActorRocket : public TActor
{
public:
	TActorRocket(const ofShapeLine2& FiringLine);

	virtual void			Update(float TimeStep,TWorld& World);
	
	virtual TCollisionShape	GetCollisionShape() const;

public:
	vec2f				mVelocity;
};
