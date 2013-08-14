#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "Hyperfeel_Button.h"

class HyperFeel_Data {
public:
	HyperFeel_Data(){};
	~HyperFeel_Data(){};

	float attention, meditation, timeStamp;
};

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
	
	//drawing options
	//
	//rainbow layers
	string drawType;
	bool bRainbowLayersIsSetup;
	void setupRainbowLayers();
	void drawRainbowLayers();
	
	//displaced mesh
	bool bDisplacedMeshIsSetup;
	void setupDisplacedMesh();
	void drawDisplacedMesh();
	ofVec3f normalFrom3Points(ofVec3f p0, ofVec3f p1, ofVec3f p2);
	ofVec3f normalFrom4Points(ofVec3f p0, ofVec3f p1, ofVec3f p2, ofVec3f p3);
	ofVbo displacedVbo;
	int displacedVertexCount, displacedIndexCount;
	ofShader displacedShader;
	
	
	
	bool bHide;
	
	//Hyper feel
	vector<HyperFeel_Data> getFakeData(int amount = 1000){
		vector<HyperFeel_Data> data( amount );
		
		for(int i=0; i<amount; i++){
			
			data[i].attention = ofNoise( float(i * 40) / (amount-1.) ) * ofNoise( float(i * 2) / (amount-1.) );
			data[i].meditation = ofNoise( float(i * 40) / (amount-1.) + 10) * ofNoise( float(i * 2) / (amount-1.) + 100);
			data[i].timeStamp = float(i) / amount;
		}
		
		
		return data;
	};
	
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