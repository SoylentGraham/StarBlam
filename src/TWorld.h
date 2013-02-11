#pragma once

#include "Main.h"
#include "TActor.h"


class TWorld
{
public:
	void		Render(float TimeStep,const TRenderSettings& RenderSettings);
	void		Update(float TimeStep);

	template<class ACTORTYPE>
	ACTORTYPE*		CreateActor();
	template<class ACTORTYPE,typename P1>
	ACTORTYPE*		CreateActor(const P1& Param1);

	void			DestroyActor(TActor* pActor);

public:
	Array<TActor*>	mActors;
};




template<class ACTORTYPE>
ACTORTYPE* TWorld::CreateActor()
{
	ACTORTYPE* pActor = new ACTORTYPE;
	if ( !pActor )
		return NULL;

	mActors.PushBack( pActor );
	return pActor;
}



template<class ACTORTYPE,typename P1>
ACTORTYPE* TWorld::CreateActor(const P1& Param1)
{
	ACTORTYPE* pActor = new ACTORTYPE( Param1 );
	if ( !pActor )
		return NULL;

	mActors.PushBack( pActor );
	return pActor;
}


