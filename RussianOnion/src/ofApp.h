#pragma once

#include "ofMain.h"
#include "ofxUI.h"
#include "ofxLibwebsockets.h"
#include "Journey.h"
#include "Onion.h"

class HyperFeel_Data {
public:
	HyperFeel_Data(){};
	~HyperFeel_Data(){};

	float attention, meditation, timeStamp;
};

class ofApp : public ofBaseApp{
	
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
	
	//rendertypes
	void drawRibbons();
	
	
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
	
	vector<ofVec3f> vData;
	void journeyToVbo(ofVbo* vbo, Journey* journey);
//	void journeyToVbo(ofVbo &vbo, Journey& journey, GLenum usage=GL_STATIC_DRAW);
	
    /**
     *  Websocekts stuff...
     */
	ofxLibwebsockets::Client client;
	
	// websocket methods
	void onConnect( ofxLibwebsockets::Event& args );
	void onOpen( ofxLibwebsockets::Event& args );
	void onClose( ofxLibwebsockets::Event& args );
	void onIdle( ofxLibwebsockets::Event& args );
	void onMessage( ofxLibwebsockets::Event& args );
	void onBroadcast( ofxLibwebsockets::Event& args );
    
    /**
     * JSON parsing stuff...
     */
	Json::Value json;
	Json::Reader reader;
	bool bSaveJsonsToFile, bLoadJsonsFromFile;
	
	ofColor getRandomColor();
	void retryColors();
	
	/**
	 * shaders
	 */
	ofShader facingRatioShader; 
	ofShader normalShader;
	ofShader onionShader;
	
	/**
	 * onion
	 */
	void setupOnion();
	void drawOnion();
	bool bOnionSetup;
	void setupSphere( float radians = PI, float sphereRad=200.f);
	ofVec3f normalFrom3Points(ofVec3f p0, ofVec3f p1, ofVec3f p2);
	ofVec3f normalFrom4Points(ofVec3f p0, ofVec3f p1, ofVec3f p2, ofVec3f p3);
	
	ofVbo sphereVbo;
	int spherVboIndexCount;
	
	vector<Onion> onions;
	
    /**
     *  Journeys
     */
	void handleRoute(Json::Value& json);
	bool bDebug;
	bool bJourniesNeedUpdate;
	void JourneyInit();
    vector<Journey*> journeys;
    vector<ofVbo*> vbos;
    vector<ofFloatColor> vboColors;
	map <string, ofColor> colorMap;
	
	ofShader* currentShader;
	vector<string> shaderNames;
	map< string, ofShader*> shaderMap;
	
	GLenum ribbonDrawType;
	
	//	ui vars
	bool bDepthTest;
	float radius;
	float nearDepthScale;
	float farDepthScale;
	float frExpo;
	float twistAngle;
	float minRibbonScale;
	float maxRibbonScale;

	
	//drawing options
	
	//Gui methods
	void loadPreset( string name);
	void savePreset( string folderName );
	void savePreset(){
		string presetName = ofSystemTextBoxDialog("Save Preset As");
		if(presetName.length())	savePreset(presetName);
		else	bSavePreset = false;
	};
	
	//animation
	float elapsedTime;
	
	//camera
	ofEasyCam camera;
	
	//meshes
	ofShader dataShader;
	
	
	//FBO
	ofFbo fbo;
	ofShader fboShader;
	ofFbo fboMap;
	
	//ofxUI
	vector<string> getPresetNames();
	vector<string> renderTypes;
	vector<string> presetNames;
	
	string currentRenderType;
	vector <ofxUICanvas*> guis;
	
	void setDefaults();
	void setupUI();
    void guiEvent(ofxUIEventArgs &e);
	ofxUICanvas* presetGui;
	ofxUIRadio* presetRadio;
	
	string currentPresetName, nextPreset;
	bool bSavePreset;
};