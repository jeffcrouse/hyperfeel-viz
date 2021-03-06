#pragma once

#include "ofMain.h"
#include "ofxUI.h"
#include "ofxLibwebsockets.h"
#include "Journey.h"
#include "Onion.h"
#include "HyperfeelTween.h"
#include "SoundManager.h"
#include "RecordManager.h"


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
	void drawMipMaps();
	
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
    float lastMouseMove;
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	
    ofSoundStream soundStream;
    void audioIn(float * input, int bufferSize, int nChannels);
    
    SoundManager soundManager;
    RecordManager recordManager;
    
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
	
	float keyVisVar;
	map < string, float > sideviewValues;
	map < string, float > currentValues;
	map < string, float > lastValues;
	
	void tweenToPreset( map<string, float>* p, float mixval );
	void tweenToPreset( string presetName, float mixval );
	
	void mixPresets( string p_0, string p_1, float mixval, map<string, float> * values );
	void mixPresets( map<string, float>* p_0, map<string, float>* p_1, float mixval, map<string, float> * values );
	void mixPresets( string p_0, string p_1,  float mixval );
	void mixPresets( string p,  float mixval );
	void mixPresets( map<string, float>* p_0, map<string, float>* p_1, float mixval );
	
	vector<ofVec3f> vData;
	void journeyToVbo(ofVbo* vbo, Journey* journey);
//	void journeyToVbo(ofVbo &vbo, Journey& journey, GLenum usage=GL_STATIC_DRAW);
	
	Reading sampleJourney( int i, float u );
	float cameraOffsetVal;
	
	void addJourneyTween();
	string keyVis;
	string activePreset;
	
    /**
     *  Websocekts stuff...
     */
    ofxLibwebsockets::ClientOptions options;
	ofxLibwebsockets::Client client;
	bool bClientInitialized;
    bool bClientConnected;
    float lastConnectionAttempt;
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
	
	
	/**
	 * color sampling...
	 */
	vector <ofFloatColor> controlColors;
	ofFloatColor getColor(float sampleVal, vector<ofFloatColor> * _controlColors );
	
	ofImage colorMapImage;
	
	ofImage roygbiv;
	ofImage redToWhite;
	ofImage cyanToWhite;
	
	bool palete_0;
	ofFloatColor journeyIntroColor;
	vector <ofxUIImageToggle*> imageButtons;
	vector<ofFloatColor> colorPalette;
	ofxUICanvas* guiColor;
	void addColorPalette( string filePath );
	float journeyColorMixLow, journeyColorMixHi;
	/**
	 * shaders
	 */
	ofShader keyVisualShader;
	ofShader post;
	ofShader facingRatioShader; 
	ofShader normalShader;
	ofShader displacedShader;
	ofShader displacedBarsShader;
	ofShader displacedDotsShader;
	ofShader onionShader;
	ofShader onionBarsShader;
	ofShader onionBarsHorzShader;
	ofShader onionDotsShader;
	ofShader particleShader;
	
	ofColor getNextJourneyColor();
	ofColor getRandomColor();
	void retryColors();
	
    bool bDebugScreen;
    ofImage debubgFill;
	/**
	 * onion
	 */
	void setupOnion();
	void drawOnion();
	bool bOnionSetup;
	void setupSphere( float radians = PI, float sphereRad=20.f);
	ofVec3f normalFrom3Points(ofVec3f p0, ofVec3f p1, ofVec3f p2);
	ofVec3f normalFrom4Points(ofVec3f p0, ofVec3f p1, ofVec3f p2, ofVec3f p3);
	
	ofVbo sphereVbo;
	int spherVboIndexCount;
	
	vector<Onion> onions;
	ofVec3f Eul;
    
	float recursiveScale;
	float squish, squishY;
	float readingThreshold;
	float readingScale;
	float onionAlpha;
	float dataSmoothing;
	float facingRatio, innerFacingRatio, outerFacingRatio;
	float displacement;
	float noiseScale;
	float slope;
	bool bRotateOnNewJourney;
	float newRibbonScaleDuration;
	float dispLow, dispHigh;
	float outerDisplacement, innerDisplacement;
	float innerReadingScale, outerReadingScale;
	float innerReadingThreshold, outerReadingThreshold;
	float innerAlpha, outerAlpha;
	float circleRadius;
	float edgeAADist;
	float glowCoefficient;
	float glowExponent;
	float glowScale;
	float noiseExponent;
	float noiseMixExponent;
	float noisePosScale;
	float noiseSpread;
	
	float journeyMixTime;
	
	float rotateX, rotateY, rotateZ;
	float positionX, positionY, positionZ;
	float onionPosX, onionPosY, onionPosZ;
	
	float tunnelMix;
	float tunnelDeltaScl;
	float tunnelTimeScl;
	float tunnelDepthScl;
	
	float vortexScl;
	
	float EulScale;
	
	string journeyTransitionTween;
	float journeyTransitionVal;
	float journeyVal;
	
	string journeyTransitionIntro;
	
	bool bLaodingJourney;
	int presetMixIndex;
	
	bool bSideView;
	void drawSideView();
	
	
	float minSampleVal;
	
	float keyVisSpan;
	
    /**
     *  Journeys
     */
	void handleRoute(Json::Value& json);
	bool bDebug;
	bool bJourniesNeedUpdate;
	void JourneyInit();
    vector<Journey*> journeys;
    vector<Journey*> queue;
    vector<ofVbo*> vbos;
    vector<ofFloatColor> vboColors;
	map <string, ofColor> colorMap;
	vector <ofColor> colorArray;
	int colorArrayIndex;
	
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
	ofMatrix4x4 globalTransform;
	float globalRotationAboutXAxis;
	string globalRotX;
	string rotatedBack;
	string addRibbonTween;
	string addRibbonScaleTween;
	
	float addRibbonVal;
	bool bAddingRibbon;
	float newRibbonScale;
	float newRibbonRotY;
	float newRibbonShaderScale;
	
	vector <string> animationPresets;
	vector <string> transitionPresets;
	
	map<string, float>* p0;
	map<string, float>* p1;
	map<string, float>* t0;
	map<string, float>* t1;
	
	float animationPresetVariationTime;
	int animationPresetIndex0, animationPresetIndex1;
	int transitionPresetIndex0, transitionPresetIndex1;
	bool bPlayAnimation;
	void tweenEventHandler(TweenEvent &e);
	TWEEN tween;
	
	void cachePresetValues();
	bool bCachedPresetValues;
	map< string, map< string, float > > presets;
//	map< string, map<string, ofxUIWidget> > presets;
	
	float variation;
	string variationKey;
	Tween * variationTween;
	
	float presetMix;
	string presetMixKey;
	
	float elapsedTime, timeScale, lasttime, timedelta;
	
	//camera
	ofEasyCam camera;
	
	//meshes
	ofShader dataShader;
	
	ofImage outImage;
	bool captrure;
	
	//FBO
	ofFbo fbo;
	ofShader fboShader;
	ofFbo fboMap;
	ofFbo fbo_mm1;
	ofFbo fbo_mm2;
	ofFbo fbo_mm3;
	ofFbo fbo_mm4;
	ofFbo fbo_mm5;
	ofFbo fbo_mm6;
	
	ofFbo sideViewFbo;
	
	ofFbo sideView_mm1;
	ofFbo sideView_mm2;
	ofFbo sideView_mm3;
	ofFbo sideView_mm4;
	ofFbo sideView_mm5;
	ofFbo sideView_mm6;
	
	ofImage noiseImage;
	
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
	
	
	//particles
	int numParticles;
	ofVbo particleVbo;
    
    float screenPosOffsetX, screenPosOffsetY;
    ofxUICanvas* screenPositionGui;
	
};