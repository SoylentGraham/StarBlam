#include "Main.h"
#include "testApp.h"
#include "ofAppGlutWindow.h"

//========================================================================
int main( )
{
	vec2f IPadRes( 2048, 1536 );
	vec2f IPhone3Res( 480, 320 );
	vec2f IPhone4Res( 960, 640 );
	vec2f IPhone5Res( 1136, 640 );
	auto Res = IPhone4Res;

    ofAppGlutWindow window;
	ofSetupOpenGL(&window, Res.x, Res.y, OF_WINDOW);			// <-------- setup the GL context

	//	core window/global setup
	window.setWindowPosition( 600, 200 );
	ofSetVerticalSync(true);

	//	line antialiasing
	ofEnableSmoothing(); 
	
	//	shape antialiasing
	window.setGlutDisplayString("rgba double samples>=4");

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp( new testApp());

}
