#include "TWorld.h"


void TWorld::Render(float TimeStep,const TRenderSettings& RenderSettings)
{
	//	render actors
	for ( int a=0;	a<mActors.GetSize();	a++ )
	{
		auto& Actor = *mActors[a];

		if ( RenderSettings.mMode == TRenderMode::Collision )
			Actor.RenderCollision( RenderSettings );
		else
			Actor.Render( TimeStep, RenderSettings );
	}
}


void TWorld::Update(float TimeStep)
{
	//	update actors (physics etc)
	for ( int a=0;	a<mActors.GetSize();	a++ )
	{
		auto& Actor = *mActors[a];
		Actor.Update( TimeStep, *this );
	}
}


void TWorld::DestroyActor(TActor* pActor)
{
	//	remove from lists
	mActors.Remove( pActor );

	delete pActor;
}


