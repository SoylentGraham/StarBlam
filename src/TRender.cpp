#include "TRender.h"




ofTrueTypeFont TRender::Font;


bool TRender::InitFont()
{
	int dpi = 150;
	int size = 10;
	bool AntiAliased = true;
	bool FullCharSet = false;
	bool makeContours = false;
	float simplifyAmt = 0.3;
	const char* Filename = "data/NewMedia Fett.ttf";

	return Font.loadFont( Filename, size, AntiAliased, FullCharSet, makeContours, simplifyAmt, dpi );
};


TRenderSceneScope::TRenderSceneScope(const char* SceneName)
{
	ofPushMatrix();
	ofPushStyle();
	ofSetupGraphicDefaults();
	ofDisableBlendMode(); 
}


TRenderSceneScope::~TRenderSceneScope()
{
	ofPopMatrix();
	ofPopStyle();
}

