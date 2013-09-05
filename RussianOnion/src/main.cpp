#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
    
    bool bUseExtendedDesktop = true;
    string cmd = "defaults write com.apple.glut GLUTUseExtendedDesktopKey ";
    cmd += bUseExtendedDesktop ? "TRUE" : "FALSE";
    std::system( cmd.c_str() );
    
    
	ofSetupOpenGL(1920*2, 1200, OF_FULLSCREEN );			// <-------- setup the GL context

    
	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());

}
