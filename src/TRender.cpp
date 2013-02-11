#include "TRender.h"



TRenderSceneScope::TRenderSceneScope(const char* SceneName)
{
	ofPushMatrix();
	ofPushStyle();
}


TRenderSceneScope::~TRenderSceneScope()
{
	ofPopMatrix();
	ofPopStyle();
}

