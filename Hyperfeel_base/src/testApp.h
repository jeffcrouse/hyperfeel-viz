#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "Hyperfeel_Button.h"

class testApp : public ofBaseApp{
	
public:
	void setup();
	void loadShaders();
	void update();
	void draw();
	
	void exit();
	
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	
	bool bHide;
	
	//Gui methods
	void addPresetToGui( string name );
	void loadPresetsToGui();
	void updateGuiPresets();
	void switchPresets();
	void loadPreset( string name);
	void savePreset();
	
	//animation
	float elapsedTime;
	
	//camera
	ofEasyCam camera;
	ofxGuiGroup cameraGroup;
	ofParameter<float> nearClip;
	ofParameter<float> farClip;
	ofParameter<float> fov;
	
	//meshes
	ofVbo vbo;
	ofShader dataShader;
	
	ofParameter<string> meshLabel;
	ofParameter<float> curveOffset;
	ofParameter<float> curveRadius;
	ofParameter<float> curveWidth;
	
	ofParameter<string> ssaoLabel;
	ofParameter<float> minThreshold;
	ofParameter<float> maxThreshold;
	ofParameter<float> sampleRadius;
	
	
	//FBO
	ofFbo fbo;
	ofShader fboShader;
	
	
	//GUI
	string currentPresetName;
	map< string, Hyperfeel_Button> presetNames;
	
	ofxButton savePresetButton;
	ofParameter<string> fpsLabel;
	ofParameter<float> radius;
	ofParameter<ofColor> color;
	ofParameter<ofColor> bckgrnd0, bckgrnd1;
	ofxPanel gui;
	ofxGuiGroup presetGroup;
	
	int numVertices;
//	vector<float> attention, meditation, timeStamp, plusMinus;
};