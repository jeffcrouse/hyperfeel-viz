#include "ofApp.h"
#include <unistd.h>

string hostname()
{
    char myhost[255];
    gethostname(myhost, (size_t)sizeof(myhost));
    return string( myhost );
}

//--------------------------------------------------------------
void ofApp::setup()
{
	ofSetVerticalSync(true);
	bDebug = true;
    
    ofSetWindowShape(1920*2,1200);
    
	//fbo
//	fbo.allocate( ofGetWidth(), ofGetHeight(), GL_RGBA16F, 4 );
	
	ofFbo::Settings s;
	s.width         = ofGetWidth()/2;
	s.height            = ofGetHeight();
	s.internalformat    = GL_RGBA;
	s.numColorbuffers   = 1;
	s.useDepth = true;
	s.numSamples = 4;
	fbo.allocate(s);
	
	//mip mapping for the glow
	fbo_mm1.allocate( fbo.getWidth()/2, fbo.getHeight()/2, GL_RGB );
	fbo_mm2.allocate( fbo_mm1.getWidth()/2, fbo_mm1.getHeight()/2, GL_RGB );
	fbo_mm3.allocate( fbo_mm2.getWidth()/2, fbo_mm2.getHeight()/2, GL_RGB );
	fbo_mm4.allocate( fbo_mm3.getWidth()/2, fbo_mm3.getHeight()/2, GL_RGB );
	fbo_mm5.allocate( fbo_mm4.getWidth()/2, fbo_mm4.getHeight()/2, GL_RGB );
	fbo_mm6.allocate( fbo_mm5.getWidth()/2, fbo_mm5.getHeight()/2, GL_RGB );
	
	//side view fbo
	ofFbo::Settings sideviewFboSettings;
	sideviewFboSettings.width         = ofGetWidth()/2;
	sideviewFboSettings.height            = ofGetHeight();
	sideviewFboSettings.internalformat    = GL_RGBA;
	sideviewFboSettings.numColorbuffers   = 1;
	sideviewFboSettings.useDepth = true;
	sideviewFboSettings.numSamples = 4;
	sideViewFbo.allocate( sideviewFboSettings );
	
	cout << "sideviewFboSettings.width: " << sideviewFboSettings.width  << endl;
	cout << "sideViewFbo.width: " << sideViewFbo.getWidth()  << endl;
	
	sideView_mm1.allocate( sideViewFbo.getWidth()/2, sideViewFbo.getHeight()/2, GL_RGB );
	sideView_mm2.allocate( sideView_mm1.getWidth()/2, sideView_mm1.getHeight()/2, GL_RGB );
	sideView_mm3.allocate( sideView_mm2.getWidth()/2, sideView_mm2.getHeight()/2, GL_RGB );
	sideView_mm4.allocate( sideView_mm3.getWidth()/2, sideView_mm3.getHeight()/2, GL_RGB );
	sideView_mm5.allocate( sideView_mm4.getWidth()/2, sideView_mm4.getHeight()/2, GL_RGB );
	sideView_mm6.allocate( sideView_mm5.getWidth()/2, sideView_mm5.getHeight()/2, GL_RGB );
	
    
	//web sockets
    bClientInitialized = false;
    bClientConnected = false;
    options = ofxLibwebsockets::defaultClientOptions();
    
    ofLogNotice() << "hostname = " << hostname();
    if(hostname()=="brainz.lan")
        options.host = "localhost";
    else if(hostname()=="brainz.local")
        options.host = "brainz.local";
    else
        options.host = "brainz.io";
    ofLogNotice() << "options.host " << options.host;
    options.port = 8080;
    client.addListener(this);
    
    ofSetFrameRate(60);
    
//    ofSetLogLevel(OF_LOG_VERBOSE);
	
	//Journy stuff
	bLoadJsonsFromFile = false;
	bJourniesNeedUpdate = false;
	bOnionSetup = false;
	
	//	journey colors
	colorMap["red"].set( 255, 99, 99 );			// = ofColor::red;
	colorMap["orange"].set( 255, 182, 42 );			// = ofColor::orange;
	colorMap["yellow"].set( 236,  236, 146 );		// = ofColor::yellow;
	colorMap["green"].set( 79, 230, 60 );			// = ofColor::green;
	colorMap["blue"].set( 4, 184, 197 );			// = ofColor::blue;
	colorMap["indigo"].set( 131, 102, 212 );		// = ofColor::indigo;
	colorMap["violet"].set( 227, 59, 207 );			// = ofColor::violet;
	
	colorArrayIndex = 0;
	colorArray.push_back( colorMap["red"] );
	colorArray.push_back( colorMap["orange"] );
	colorArray.push_back( colorMap["yellow"] );
	colorArray.push_back( colorMap["green"] );
	colorArray.push_back( colorMap["blue"] );
	colorArray.push_back( colorMap["indigo"] );
	colorArray.push_back( colorMap["violet"] );
	
	controlColors.resize(64);
	for (int i=0; i<controlColors.size(); i++) {
		float val = float(i) / float( controlColors.size()-1 );
		controlColors[i].set( val, val, val, 1. );
	}
	
	colorMapImage.loadImage( "GUI/defaultColorPalette.png" );
	

	// -
	loadShaders();
	
	// ---
	setDefaults();
	
    // ---
	setupUI();
	
	currentPresetName = "default";
	
	if(bLoadJsonsFromFile){
		
		//hackey population of journeys
		int file_index=0;
		for(int i=0; i<35; i++){
			ofBuffer buffer = ofBufferFromFile("Journeys/journey_showJourney" + ofToString(file_index) + ".json");
			if(buffer.size()){
				reader.parse( buffer.getText(), json );
				handleRoute( json );
				file_index++;
			}
			else{
				file_index = 0;
			}
		}
	}
	
	//animation
	//TODO: rename "newRibbonScaleDuration"
	newRibbonScaleDuration = 45; // <---- this controls the time it takes for the journey to animate in.

	animationPresetVariationTime = 6;
	animationPresetIndex0 = 0;
	animationPresetIndex1 = 1;
	
	animationPresets.push_back("k_0");
	animationPresets.push_back("keyVis");
	animationPresets.push_back("k_0");
	animationPresets.push_back("k_12");
	animationPresets.push_back("k_1");
	animationPresets.push_back("keyVis");
	animationPresets.push_back("k_3");
	animationPresets.push_back("k_4");
	animationPresets.push_back("k_5");
	animationPresets.push_back("k_6");
	animationPresets.push_back("k_7");
	animationPresets.push_back("k_8");
	animationPresets.push_back("keyVis");
	animationPresets.push_back("k_9");
	animationPresets.push_back("k_10");
	animationPresets.push_back("k_11");
	animationPresets.push_back("k_12");
	animationPresets.push_back("k_13");
	animationPresets.push_back("keyVis");
	animationPresets.push_back("k_14");
	animationPresets.push_back("k_15");
	animationPresets.push_back("k_16");
	animationPresets.push_back("keyVis");
	animationPresets.push_back("t_0");
	animationPresets.push_back("t_1");
	animationPresets.push_back("t_3");
	animationPresets.push_back("t_4");
	animationPresets.push_back("keyVis");
	animationPresets.push_back("t_5");
	animationPresets.push_back("t_6");
	animationPresets.push_back("t_7");
	animationPresets.push_back("t_8");
	animationPresets.push_back("keyVis");
	animationPresets.push_back("t_66");
	animationPresets.push_back("t_67");
	animationPresets.push_back("t_68");
	animationPresets.push_back("t_69");
	animationPresets.push_back("keyVis");
	
	transitionPresetIndex0 = 0;
	transitionPresets.push_back("k_1");
	transitionPresets.push_back("t_0");
	transitionPresets.push_back("t_1");
	transitionPresets.push_back("t_3");
	transitionPresets.push_back("k_2");
	transitionPresets.push_back("t_4");
	transitionPresets.push_back("t_5");
	transitionPresets.push_back("k_12");
	transitionPresets.push_back("t_6");
	transitionPresets.push_back("t_7");
	transitionPresets.push_back("k_7");
	transitionPresets.push_back("t_8");
	transitionPresets.push_back("t_66");
	transitionPresets.push_back("t_67");
	transitionPresets.push_back("t_68");
	transitionPresets.push_back("k_5");
	transitionPresets.push_back("t_69");

	
	keyVis = "keyVis";
	
	bPlayAnimation = true;
	
	//kick off animation variation
	variationKey = tween.addTween( variation, 0, 1, ofGetElapsedTimef(), ofGetElapsedTimef(), "variation", TWEEN_SINUSOIDAL, TWEEN_INOUT);
	variationTween = tween.getTween( variationKey );//<--a looping tween( basically a timer ) that triggers transitions between presets
	variationTween->bKeepAround = true;
	
	bAddingRibbon = false;
	newRibbonScale = 1;
    
	//add tween listener
	ofAddListener( TweenEvent::events, this, &ofApp::tweenEventHandler );
    
    //soundStream.listDevices();
    //soundStream.setDeviceID(5);
    soundStream.setup(this, 0, recordManager.channels, recordManager.sampleRate, 256, 4);
	
	
	//camera
	
	
	vector <ofVec3f> particlePositions;
	numParticles = 20000;
	particlePositions.resize(numParticles);
	
	for (int i=0; i<numParticles; i++) {
//		particlePositions[i].set( ofRandom(-1, 1), ofRandom(-1, 1), ofRandom(-1, 1) );
		particlePositions[i].set( ofRandom(-1024, 1024), ofRandom(-1024, 1024), ofRandom(-1024, 1024) );
	}
	
	particleVbo.setVertexData( &particlePositions[0], particlePositions.size(), GL_STATIC_DRAW );
    
    debubgFill.loadImage("palettes/red.png");
    
    bDebugScreen = false;
}


//--------------------------------------------------------------
void ofApp::audioIn(float *input, int bufferSize, int nChannels)
{
    recordManager.audioIn(input, bufferSize, nChannels);
}

//--------------------------------------------------------------
void ofApp::setDefaults()
{
	camera.setFarClip(20000);
	camera.setNearClip(20);
	timeScale = 1.;
	radius = 10;
	nearDepthScale = 3000;
	farDepthScale = 3500;
	
	bDepthTest = true;
	
	bLaodingJourney = false;
	
	circleRadius = ofGetHeight() / 2;
	edgeAADist = 2;
	glowCoefficient = .5;
	glowExponent = 2;
	glowScale = .5;
	
	
	rotateX = 0;
	rotateY = 0;
	rotateZ = 0;
	
	slope = .05;
	bRotateOnNewJourney = false;
	newRibbonShaderScale = 1;
	
	journeyMixTime = 4;
	
	noiseExponent = 2;
	noiseMixExponent = 4;
	noisePosScale = 100;
	noiseSpread = -.25;
	
	squish = squishY = 1.;
	onionPosX = onionPosY = onionPosZ = 0;
	tunnelMix = 1.;
	tunnelDeltaScl = .1;
	tunnelTimeScl = 4.;
	tunnelDepthScl = 15.;
	
	EulScale = 1.;
	
	bSideView = false;
	
	keyVisSpan = 2;
	
	vortexScl = 1;
    
    
    screenPosOffsetX = 0;
    screenPosOffsetY = 0;
}

void ofApp::setupUI()
{
	
	renderTypes.clear();
	renderTypes.push_back("onion");
	
    int columnWidth = 75;
	
	float dim = 24;
	float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
    float length = 220-xInit;
	
	setDefaults();
    
    
	screenPositionGui = new ofxUICanvas(columnWidth, 0, length+xInit, columnWidth);
	screenPositionGui->setName("Screen");
    screenPositionGui->addSlider( "screenPositionX", -100., 100., &screenPosOffsetX );
    screenPositionGui->addSlider( "screenPositionY", -100., 100., &screenPosOffsetY );
    screenPositionGui->setPosition( 10, ofGetHeight() / 2 );
    
	
	ofxUICanvas* guiMain = new ofxUICanvas(columnWidth, 0, length+xInit, columnWidth);
	guiMain->setName("Main");
	guiMain->setFont("GUI/OpenSans-Semibold.ttf");
    guiMain->setFontSize(OFX_UI_FONT_LARGE, 6);
    guiMain->setFontSize(OFX_UI_FONT_MEDIUM, 6);
    guiMain->setFontSize(OFX_UI_FONT_SMALL, 6);
	
	guiMain->addLabel("hit 'h' to hide/show ui");
	
	guiMain->setPosition(10, 10);
	guiMain->setColorFill(ofxUIColor(200));
	guiMain->setColorFillHighlight(ofxUIColor(255));
	guiMain->setColorBack(ofxUIColor(20, 20, 20, 100));

	guiMain->addFPS();
	guiMain->addSpacer();
	guiMain->addSlider("timeScl", -5, 5, &timeScale );
	ofxUIToggle* toggle = guiMain->addToggle("playAnimation", &bPlayAnimation );
	guiMain->addSlider("circleRadius", 450, 460, &circleRadius );
	guiMain->addSlider("edgeAADist", 1, 10, &edgeAADist );
	guiMain->addSlider("vortexScl", -1, 1, &vortexScl );
	
	guiMain->addLabel("render Types");
	guiMain->addRadio("renderTypes", renderTypes );
	
	guiMain->addLabel("main");

	guiMain->addSpacer();
	guiMain->addLabel("global rendering");
	guiMain->addToggle("depthTest", &bDepthTest);
	
	//create a radio for switching renderTypes
	guiMain->addSpacer();
	guiMain->addLabel("Shaders" );
//	cout << "shaderNames.size(): "<< shaderNames.size() << endl;
	guiMain->addRadio("shaders", shaderNames );
	
	
    //guiMain->addSpacer();
    //guiMain->addWidgetDown( new ofxUIBaseDraws(320, 240, &soundManager.audioLevelsPreview, "AUDIO LEVELS", true) );

	guiMain->autoSizeToFitWidgets();
	
	ofxUICanvas* guiPost = new ofxUICanvas(columnWidth, 0, length+xInit, columnWidth);
	guiPost->setName("Post");
	guiPost->setFont("GUI/OpenSans-Semibold.ttf");
	guiPost->setFontSize(OFX_UI_FONT_LARGE, 6);
	guiPost->setFontSize(OFX_UI_FONT_MEDIUM, 6);
	guiPost->setFontSize(OFX_UI_FONT_SMALL, 6);
	guiPost->setColorFill(ofxUIColor(200));
	guiPost->setColorFillHighlight(ofxUIColor(255));
	guiPost->setColorBack(ofxUIColor(20, 20, 20, 100));
	guiPost->setPosition( 10 + guiMain->getRect()->getWidth() + guiMain->getRect()->getX(), 10);
	
	guiPost->addLabel("POST PROCESSING");
	guiPost->addSlider("glowCoefficient", 0., 1., &glowCoefficient);
	guiPost->addSlider("glowExponent", 1., 10., &glowExponent);
	guiPost->addSlider("glowScale", 0., 4., &glowScale);
	
	guiPost->autoSizeToFitWidgets();
	
    //
    //  Utils
    //
    ofxUICanvas* guiUtils = new ofxUICanvas(columnWidth, 0, length+xInit, columnWidth);
    guiUtils->setName("Utils");
	guiUtils->setFont("GUI/OpenSans-Semibold.ttf");
	guiUtils->setFontSize(OFX_UI_FONT_LARGE, 6);
	guiUtils->setFontSize(OFX_UI_FONT_MEDIUM, 6);
	guiUtils->setFontSize(OFX_UI_FONT_SMALL, 6);
	guiUtils->setColorFill(ofxUIColor(200));
	guiUtils->setColorFillHighlight(ofxUIColor(255));
	guiUtils->setColorBack(ofxUIColor(20, 20, 20, 100));
    
    guiUtils->addLabel("Utils");
    guiUtils->addSpacer();
    guiUtils->addToggle("Mute Audio", &soundManager.bMuted);
    guiUtils->addToggle("Make Snapshots", &recordManager.bMakeSnapshots);
    guiUtils->addToggle("Make Videos", &recordManager.bMakeVideo);
    guiUtils->addToggle("Make Photo Strips", &recordManager.bMakePhotoStrips);
    guiUtils->addSpacer();
    guiUtils->addWidgetDown( new ofxUIBaseDraws(320, 240, &soundManager.audioLevelsPreview, "AUDIO LEVELS", true) );
    
    guiUtils->autoSizeToFitWidgets();
    guiUtils->setPosition( guiMain->getRect()->getX(), ofGetHeight()-guiUtils->getRect()->getHeight()-20);

    //
    //  Shader
    //

	
	ofxUICanvas* guiShader = new ofxUICanvas(columnWidth, 0, length+xInit, columnWidth);
	guiShader->setName("Shader");
	guiShader->setFont("GUI/OpenSans-Semibold.ttf");
	guiShader->setFontSize(OFX_UI_FONT_LARGE, 6);
	guiShader->setFontSize(OFX_UI_FONT_MEDIUM, 6);
	guiShader->setFontSize(OFX_UI_FONT_SMALL, 6);
	guiShader->setColorFill(ofxUIColor(200));
	guiShader->setColorFillHighlight(ofxUIColor(255));
	guiShader->setColorBack(ofxUIColor(20, 20, 20, 100));
	guiShader->setPosition( guiPost->getRect()->getX(), guiPost->getRect()->getY() + guiPost->getRect()->getHeight() + 10);
	guiShader->setWidth( 350 );
	
	guiShader->addLabel("SHADER");
	guiShader->addSlider("radius", 1, 30, &radius );
	guiShader->addSlider("recursiveScale", .5, 1., &recursiveScale );
	guiShader->addSlider("squish", .01, 1., &squish );
	guiShader->addSlider("squishY", .01, 1., &squishY );
	guiShader->addSlider("dataSmoothing", .01, 1., &dataSmoothing );
	
	guiShader->addSlider("outerAlpha", .01, 1., &outerAlpha );
	guiShader->addSlider("innerAlpha", .01, 1., &innerAlpha );
	
	guiShader->addSlider("innerFacingRatio", .01, 1., &innerFacingRatio );
	guiShader->addSlider("outerFacingRatio", .01, 1., &outerFacingRatio );
	guiShader->addSlider("noiseScale", 0, .025, &noiseScale );
	guiShader->addSlider("slope", 0., .2, &slope );
	guiShader->addSlider("outerDisplacement", -150, 150, &outerDisplacement );
	guiShader->addSlider("innerDisplacement", -150, 150, &innerDisplacement );
	
	guiShader->addSlider("outerReadingThreshold", .0, 1., &outerReadingThreshold );
	guiShader->addSlider("innerReadingThreshold", .0, 1., &innerReadingThreshold );
	
	guiShader->addSlider("outerReadingScale", .01, 1., &outerReadingScale );
	guiShader->addSlider("innerReadingScale", .01, 1., &innerReadingScale );
	
	guiShader->addSlider("noiseExponent", 1., 10., &noiseExponent );
	guiShader->addSlider("noiseMixExponent", 1., 10., &noiseMixExponent );
	guiShader->addSlider("noisePosScale", 1., 300., &noisePosScale );
	guiShader->addSlider("noiseSpread", -1., 1., &noiseSpread );
	
	guiShader->autoSizeToFitWidgets();
	
	
	//get our presets and them to the radio
	presetGui = new ofxUICanvas();
	presetGui->setName("COLOR");
	presetGui->setFont("GUI/OpenSans-Semibold.ttf");
	presetGui->setFontSize(OFX_UI_FONT_LARGE, 4);
	presetGui->setFontSize(OFX_UI_FONT_MEDIUM, 4);
	presetGui->setFontSize(OFX_UI_FONT_SMALL, 4);
	presetGui->setColorFill(ofxUIColor(200));
	presetGui->setColorFillHighlight(ofxUIColor(255));
	presetGui->setColorBack(ofxUIColor( 90, 90, 90, 170));
	
	//save button
	presetGui->addSpacer();
	presetGui->addButton("save preset", false);
	presetGui->addSpacer();
	
	presetGui->setName("PRESETS");
	presetGui->addLabel("Presets");
	presetGui->setPosition( guiShader->getRect()->getX() + guiShader->getRect()->getWidth() + 10, 10 );
	presetGui->addSpacer();
//	presetRadio = presetGui->addRadio("presets", getPresetNames(), OFX_UI_ORIENTATION_VERTICAL );
	presetRadio = presetGui->addRadio("presets", getPresetNames(), OFX_UI_ORIENTATION_VERTICAL, 100, 10, 0,0 );
	
	presetGui->autoSizeToFitWidgets();
	
	
	//COLOR
	guiColor = new ofxUICanvas(columnWidth, 0, length+xInit, columnWidth);
	guiColor->setName("COLOR");
	guiColor->setFont("GUI/OpenSans-Semibold.ttf");
	guiColor->setFontSize(OFX_UI_FONT_LARGE, 6);
	guiColor->setFontSize(OFX_UI_FONT_MEDIUM, 6);
	guiColor->setFontSize(OFX_UI_FONT_SMALL, 6);
	guiColor->setColorFill(ofxUIColor(200));
	guiColor->setColorFillHighlight(ofxUIColor(255));
	guiColor->setColorBack(ofxUIColor( 90, 90, 90, 170));
	guiColor->setPosition( presetGui->getRect()->getX() + presetGui->getRect()->getWidth() + 10, 10 );
	guiColor->setWidth( 150 );

	
	guiColor->addLabel("COLOR");
	guiColor->addRangeSlider( "journeyColorRange", 0, 1, &journeyColorMixLow, &journeyColorMixHi );
	
	ofxUIImageSampler* journeySampler = guiColor->addImageSampler("journeyIntroColor", &colorMapImage, 100, 100);

	//invisible sliders. for tweening we need floats
	ofxUISlider* slider;
	slider = guiColor->addSlider( "journeyIntroColor_r", 0, 1, &journeyIntroColor.r, 0, 0 );
	slider->setVisible( false );
	slider = guiColor->addSlider( "journeyIntroColor_g", 0, 1, &journeyIntroColor.g, 0, 0 );
	slider->setVisible( false );
	slider = guiColor->addSlider( "journeyIntroColor_b", 0, 1, &journeyIntroColor.b, 0, 0 );
	slider->setVisible( false );
	
	string palette_path = "palettes/";
	ofDirectory palette_dir(palette_path);
	palette_dir.listDir();
//	palette_dir.allowExt(".png");
	
	//go through and store our preset names
	for(int i = 0; i < palette_dir.numFiles(); i++){
		addColorPalette( palette_dir.getPath(i) );
	}
	
	//create a bunch of hidden sliders for storing our color info. we need sliders for mixing between presets

	for (int i=0; i<controlColors.size(); i++) {
		string colorName = "c_" + ofToString(i);
		slider = guiColor->addSlider( colorName + "_r", 0, 1, &controlColors[i].r, 0, 0 );
		slider->setVisible( false );

		slider = guiColor->addSlider( colorName + "_g", 0, 1, &controlColors[i].g, 0, 0 );
		slider->setVisible( false );
		
		slider = guiColor->addSlider( colorName + "_b", 0, 1, &controlColors[i].b, 0, 0 );
		slider->setVisible( false );
	}
	
	guiColor->autoSizeToFitWidgets();
	
	
	
	//CAMERA
	ofxUICanvas* guiCamera = new ofxUICanvas(columnWidth, 0, length+xInit, columnWidth);
	guiCamera->setName("CAMERA");
	guiCamera->setFont("GUI/OpenSans-Semibold.ttf");
	guiCamera->setFontSize(OFX_UI_FONT_LARGE, 6);
	guiCamera->setFontSize(OFX_UI_FONT_MEDIUM, 6);
	guiCamera->setFontSize(OFX_UI_FONT_SMALL, 6);
	guiCamera->setColorFill(ofxUIColor(200));
	guiCamera->setColorFillHighlight(ofxUIColor(255));
	guiCamera->setColorBack(ofxUIColor( 90, 90, 90, 170));
	guiCamera->setPosition( guiColor->getRect()->getX() + guiColor->getRect()->getWidth() + 10, 10 );
	guiCamera->setWidth( 350 );
	
	guiCamera->addLabel("CAMERA");
	
	//	rotateX, rotateY, rotateZ
	guiCamera->addSlider( "rotateX", -180, 180, &rotateX );
	guiCamera->addSlider( "rotateY", -180, 180, &rotateY );
	guiCamera->addSlider( "rotateZ", -180, 180, &rotateZ );
	
	guiCamera->addSlider( "positionX", -50, 50, &positionX );
	guiCamera->addSlider( "positionY", -50, 50, &positionY );
	guiCamera->addSlider( "positionZ", -50, 50, &positionZ );
	
	guiCamera->addToggle("bSideView", &bSideView ); 
	
//	guiCamera->addSlider( "onionPosX", -50, 50, &onionPosX );
//	guiCamera->addSlider( "onionPosY", -50, 50, &onionPosY );
//	guiCamera->addSlider( "onionPosZ", -50, 50, &onionPosZ );

	guiCamera->addSlider( "tunnelMix", 0, 1, &tunnelMix );
	guiCamera->addSlider( "tunnelDeltaScl", 0, 1, &tunnelDeltaScl );
	guiCamera->addSlider( "tunnelTimeScl", -10, 10, &tunnelTimeScl );
	guiCamera->addSlider( "tunnelDepthScl", 1, 30, &tunnelDepthScl );
	
	guiCamera->addSlider("EulScale", -2., 2., &EulScale);
	
//	guiCamera->addImage("noiseImage", &noiseImage, 255, 255 );
	
	guiCamera->autoSizeToFitWidgets();

	
	//hold on to pointers for saving 'n stuff
	guis.push_back( guiMain );
	guis.push_back( presetGui );
	guis.push_back( guiShader );
	guis.push_back( guiCamera );
	guis.push_back( guiColor );
	guis.push_back( guiPost );
	guis.push_back( guiUtils );
	guis.push_back( screenPositionGui );
	
	//add listeners
	ofAddListener( guiMain->newGUIEvent,this,&ofApp::guiEvent );
	ofAddListener( presetGui->newGUIEvent,this,&ofApp::guiEvent );
	ofAddListener( guiShader->newGUIEvent,this,&ofApp::guiEvent );
	ofAddListener( guiCamera->newGUIEvent,this,&ofApp::guiEvent );
	ofAddListener( guiColor->newGUIEvent,this,&ofApp::guiEvent );
	ofAddListener( guiPost->newGUIEvent,this,&ofApp::guiEvent );
	ofAddListener( guiUtils->newGUIEvent,this,&ofApp::guiEvent );

	
	//load our working sttings
	//	loadPreset("Working");
	nextPreset = "Working";
	bCachedPresetValues = false;
}

void ofApp::addColorPalette( string filePath )
{
	ofxUIImageToggle* palette0 =  guiColor->addImageToggle( "palette_" + ofToString(imageButtons.size()),  filePath, &palete_0, 100, 10 );
	imageButtons.push_back( palette0 );
}

void ofApp::guiEvent(ofxUIEventArgs &e)
{
	
	string name = e.widget->getName();
	int kind = e.widget->getKind();
	//	cout << endl << name << " : " << kind << " : " << e.widget->getParent()->getName() <<  endl;
	

	if( name == "save preset" ){
		bSavePreset = true;
		//savePreset();
	}
	
	else{
		
		bool bFoundIt = false;
		
		//check and change the render type
		for (int i=0; i<renderTypes.size(); i++) {
			if(name == renderTypes[i]){
				//bFoundIt = true;
				if(e.getToggle()->getValue()){
					currentRenderType = name;
					//					bFoundIt = true;
				}
			}
		}
		
		//look in the presets
		if(!bFoundIt){
			presetNames = getPresetNames();
			for (int i=0; i<presetNames.size(); i++) {
				if( name == presetNames[i] ){
					//bFoundIt = true;
					if(e.getToggle()->getValue()){
						nextPreset = name;
						//						bFoundIt = true;
					}
				}
			}
		}
		
		//look in the shaders
		if(!bFoundIt){
			for(int i=0; i<shaderNames.size(); i++){
				if(name == shaderNames[i]){
					//bFoundIt = true;
					if(e.getToggle()->getValue()){
						if(shaderMap.find(name) != shaderMap.end()){
							currentShader = shaderMap[name];
						}
					}
				}
			}
		}
		
		//look in the render
		if(!bFoundIt){
			for(int i=0; i<renderTypes.size(); i++){
				if(name == renderTypes[i]){
					if(e.getToggle()->getValue()){
						//bFoundIt = true;
						currentRenderType = name;
						
//						cout << "currentRenderType: " << currentRenderType << endl;
					}
				}
			}
		}
		
		if (name == "journeyIntroColor")
		{
			ofxUIImageSampler* sampler = (ofxUIImageSampler*)e.widget;
			
			ofFloatColor col = sampler->getColor();
			
			((ofxUISlider* )guiColor->getWidget( "journeyIntroColor_r" ))->setValue( col.r );
			((ofxUISlider* )guiColor->getWidget( "journeyIntroColor_g" ))->setValue( col.g );
			((ofxUISlider* )guiColor->getWidget( "journeyIntroColor_b" ))->setValue( col.b );
		}
		
		//look in the conrolColors
		if(!bFoundIt)
		{
			for(int i=0; i<imageButtons.size(); i++)
			{
				if(name == imageButtons[i]->getName() )
				{
					ofxUIImageButton* button = (ofxUIImageButton *) e.widget;
					
//					cout << imageButtons[i]->getName() << ": " << button->getValue() << endl;
					if(button->getValue())
					{
						//set our palette to true
						button->setValue( true );
						
						//set the others to false
						for (int j=0; j<imageButtons.size(); j++)
						{
							if(i != j){
								imageButtons[j]->setValue( false );
							}
						}
						
						//now load all the colorSamples into our hidden sliders which intern update our controlColor array
						ofImage* img = button->getImage();
						int imgW = img->getWidth();
						
						float step = 1. / float(controlColors.size()-1);
						ofxUISlider* slider;
						string colorName;
						ofFloatColor col;
						for (int j=0; j<controlColors.size(); j++)
						{	
							colorName = "c_" + ofToString(j);
							col = ofFloatColor( img->getColor( (imgW-1) * step * j, 0) );

							((ofxUISlider* )guiColor->getWidget( colorName + "_r" ))->setValue( col.r );
							((ofxUISlider* )guiColor->getWidget( colorName + "_g" ))->setValue( col.g );
							((ofxUISlider* )guiColor->getWidget( colorName + "_b" ))->setValue( col.b );
						}
					}
				}
			}
		}
	}
}

//this is the engin that drives the animation
void ofApp::tweenEventHandler(TweenEvent &e)
{
	
	
	//TODO: make some sliders
	float newRibbonAnimationSpan = bRotateOnNewJourney? .5 : .01;
	
	float startScale = 1. / recursiveScale;
	
	
	//action when a new ribbon animates in
	if(e.name == addRibbonTween)
	{
		if( e.message == "started" )
		{
			if(!bAddingRibbon)
			{
				bAddingRibbon = true;
				
				//tells the shader where to clip the data texture
				addRibbonScaleTween = tween.addTween( newRibbonShaderScale, 0, 1, ofGetElapsedTimef(), newRibbonScaleDuration, "newRibbonShaderScale", TWEEN_SINUSOIDAL, TWEEN_OUT );
				
				//scales up the outside mesh to prevent scale popping caused recurssivly scaling the array of onions.
				//when we add a new layer the scales after the new layer sholud stay the same
				tween.addTween( newRibbonScale, startScale, 1, ofGetElapsedTimef(), newRibbonScaleDuration, "newRibbonScale", TWEEN_LINEAR, TWEEN_INOUT );
				newRibbonShaderScale = 0;
                
                
				//audio
                float totalDuration = newRibbonScaleDuration + (newRibbonAnimationSpan*2);
                recordManager.startJourney(journeys.back(), totalDuration);
                soundManager.startJourney(journeys.back());
			}
            bPlayAnimation = false;
		}
		
        if( e.message=="updated")
        {
            soundManager.updateJorney(journeys.back(), *tween.getTween(addRibbonTween)->value);
        }
		
		if ( e.message == "updated") {
//			cout << e.name << ": "<< e.value << " ::: " << sampleJourney(journeys.size()-1, e.value) << endl;
			Reading sample = sampleJourney(journeys.size()-1, e.value);
			cameraOffsetVal = sample.getAttention();// - sample.getMeditation();
			cameraOffsetVal *= -3;
		}
        
		if( e.message == "ended" )
		{
			bAddingRibbon = false;
			
			//Jeff, this is the correct setup?
            recordManager.endJourney(journeys.back());
            soundManager.endJourney(journeys.back());
            
            bPlayAnimation = true;
		}
	}
	
    
	//if the ribbon is done animating in remove the oldest journey & onion
	if(e.name == addRibbonScaleTween && e.message == "ended"){
        
		
		//TODO: magic number
		//remove the old journeys & onions
		
		int onionDeleteCount = 0;
		while(onions.size() > 30){ // could be while() here?
			onions.erase( onions.begin() );
			journeys.erase( journeys.begin() );
			if(onionDeleteCount > 30){
				break;
			}
			onionDeleteCount ++;
		}
	}

	
//	//this adds a journey and controls the variable that plays the music
//	if( e.name == "JourneyIntro")
//	{
//		if( e.message == "started" )
//		{
//			bLaodingJourney = true;
//			
////			//start our journey transition meedly tween. create it if it doesn't exist
////			Tween* t = tween.getTween("journeyMix");
////			if(t == NULL)
////			{
////				//cout << "creating tween journeyMix" << endl;
////				tween.addTween(journeyTransitionVal, 0, 1, ofGetElapsedTimef(), journeyMixTime, "journeyMix" );
////			}
////			else
////			{
////				t->setup( &journeyTransitionVal, 0, 1, ofGetElapsedTimef(), journeyMixTime, TWEEN_SINUSOIDAL, TWEEN_INOUT, "journeyMix" );
////			}
//		}
//		
//		if( e.message == "ended" )
//		{
//			//play the perpetual transitions again
//			//			bPlayAnimation = true;
//			lastValues = currentValues;
//			
//			tween.addTween( keyVisVar, 0, 1, ofGetElapsedTimef(), keyVisSpan, "endJournyIntro" );
//		}
//	}
	
//	if( e.name == "startJournyIntro" )
//	{
//		if (e.message == "started") {
//			lastValues = currentValues;
//			bLaodingJourney = true;
//		}
//		if (e.message == "updated") {
//			cout << e.name << ": " <<  e.value << endl;
//			mixPresets("keyVis", e.value );
//		}
//		if (e.message == "ended") {
//			
//			lastValues = currentValues;
//			
//			//start the preset mix medley 
////			tween.addTween( journeyVal, 0, 1, ofGetElapsedTimef(), newRibbonScaleDuration - keyVisSpan, "JourneyIntro" );
//		}
//	}
	
//	if( e.name == "endJournyIntro" )
//	{
//		if (e.message == "started") {
//			lastValues = currentValues;
//		}
//		if (e.message == "updated") {
//			cout << e.name << ": " <<  e.value << endl;
//			mixPresets("keyVis", e.value );
//		}
//		if (e.message == "ended") {
//			
//			lastValues = currentValues;
//			bLaodingJourney = false;
//		}
//	}
	
//	//journey transition meedly tween. plays while the new journey is loading
//	if (e.name == "journeyMix")
//	{
//		if (e.message == "started")
//		{
//			//making doublely sure that we're tweening from the most recent values
//			lastValues = currentValues;
//		}
//		
//		if(e.message == "updated")
//		{
//			//mix the target preset with our lastValues
//			mixPresets( transitionPresets[transitionPresetIndex0], e.value );
//		}
//		
//		if(e.message == "ended")
//		{
//			
//			//if we're still journeying the keep on mixing
//			Tween* jIntro = tween.getTween( "JourneyIntro" );
//			if( jIntro != NULL &&  jIntro->endTime > ofGetElapsedTimef() + journeyMixTime*2 )
//			{
//				Tween* t = tween.getTween("journeyMix");
//				t->setup( &journeyTransitionVal, 0, 1, ofGetElapsedTimef(), journeyMixTime, TWEEN_SINUSOIDAL, TWEEN_INOUT, "journeyMix" );
//				t->bKeepAround = true;
//				
//				transitionPresetIndex0++;
//				if(transitionPresetIndex0 >= transitionPresets.size() )	transitionPresetIndex0 = 0;
//			}
//			
//			//otherwise end the mix
//			else
//			{
//				//cout  << "stop journey mixing cus we're at the end of the journey intro" << endl;
//				float span = jIntro->endTime - ofGetElapsedTimef();
//				tween.addTween( keyVisVar, 0, 1, ofGetElapsedTimef(), span, "keyVisTween" );
//				
//				//start our mixing again AFTER the keyvisual has been restored ==  ofGetElapsedTimef() + span, ...
//				variationTween->setup( &variation, 0, 1, ofGetElapsedTimef() + span, animationPresetVariationTime, TWEEN_SINUSOIDAL, TWEEN_INOUT, "variation" );
//				variationTween->bKeepAround = true;
//			}
//			
//			//store the lastValues for future mixing
//			lastValues = currentValues;
//		}
//	}
	
	
	//perpetual mixing of presets
	//TODO: rename variationKey
	if(e.name == variationKey && bPlayAnimation)
	{
		if (e.message == "started")
		{
			//make doublely sure
			lastValues = currentValues;
            cout << animationPresets[presetMixIndex] << endl;
            
            cout << "STARTED ANIMATION" << endl;
		}
		
		//if we're not loading a Journey and actively animating then we are mixing and tweening
		if(!bLaodingJourney)
		{
			if( e.message == "updated")
			{
				//transition to the next preset
				mixPresets( animationPresets[presetMixIndex], e.value );
			}
			
			
			if( e.message == "ended")
			{
				//cout << e.name << ": " << e.message << " : " << ofGetElapsedTimef() << endl;
				variationTween->setup( &variation, 0, 1, ofGetElapsedTimef(), animationPresetVariationTime, TWEEN_SINUSOIDAL, TWEEN_INOUT, "variation" );
				variationTween->bKeepAround = true;
				
				presetMixIndex++;
				if (presetMixIndex >= animationPresets.size())
				{
					presetMixIndex = 0;
				}
				
				//make sure to store our current values to mix with later
				lastValues = currentValues;
			}
		}
		
		else
		{
			//we must be loading a journey. so no mixing here.
		}
	}
}


void ofApp::addJourneyTween()
{
	addRibbonTween = tween.addTween( addRibbonVal, 0, 1, ofGetElapsedTimef(), newRibbonScaleDuration, "addRibbonTween" );
	
	//this goes to keyVis -> transition medley - > keyVis over newRibbonScaleDuration
	tween.addTween( keyVisVar, 0, 1, ofGetElapsedTimef(), keyVisSpan, "startJourneyIntro" );
}

void ofApp::mixPresets( map<string, float>* p_0, map<string, float>* p_1, float mixval ){
	mixPresets( p_0, p_1, mixval, &currentValues);
}


void ofApp::mixPresets( string p_0, string p_1, float mixval, map<string, float> * values )
{
	mixPresets( &presets[p_0], &presets[p_1], mixval, values );
}

void ofApp::mixPresets( map<string, float>* p_0, map<string, float>* p_1, float mixval, map<string, float> * values )
{
	if( p_0 != NULL && p_1 != NULL )
	{
		ofxUIWidget *w;
		for (map<string, float>::iterator it=p_0->begin(); it!=p_0->end(); it++)
		{
			for (int i=0; i<guis.size(); i++)
			{
				w = guis[i]->getWidget( it->first );
				
				if(w != NULL && (*p_0).find( it->first ) != (*p_0).end() && (*p_1).find( it->first ) != (*p_1).end() )
				{
					(*values)[it->first] = ofMap( mixval, 0, 1, (*p_0)[it->first], (*p_1)[it->first], false );
					
					((ofxUISlider *)w)->setValue( (*values)[it->first] );
				}
			}
		}
	}
}


void ofApp::mixPresets( string p_0, string p_1, float mixval )
{
	mixPresets( &presets[p_0], &presets[p_1], mixval );
}


void ofApp::mixPresets( string p, float mixval )
{
	mixPresets( &lastValues, &presets[p], mixval );
}

//--------------------------------------------------------------
void ofApp::loadShaders()
{
	cout<<endl<<endl<< "loading shaders: " << ofGetFrameNum() <<endl<<endl;
	
	//load postprocess shaders
	post.load( "shaders/post" );
	
	//load data shader
	keyVisualShader.load( "shaders/keyVisual" );
	shaderMap["keyVisualShader"] = &keyVisualShader;
	
	displacedShader.load( "shaders/displaced" );
	shaderMap["displacedShader"] = &displacedShader;
	
	displacedBarsShader.load( "shaders/displacedBars" );
	shaderMap["displacedBarsShader"] = &displacedBarsShader;
	
	displacedDotsShader.load( "shaders/displacedDots" );
	shaderMap["displacedDotsShader"] = &displacedDotsShader;
	
	displacedShader.load( "shaders/displaced" );
	shaderMap["displacedShader"] = &displacedShader;
	
	particleShader.load( "shaders/ambientParticle" );
	shaderMap["particleShader"] = &particleShader;
	
//	onionShader.load( "shaders/onion" );
//	shaderMap["onionShader"] = &onionShader;
//	
//	onionBarsShader.load( "shaders/onionBars" );
//	shaderMap["onionBarsShader"] = &onionBarsShader;
//	
//	onionBarsHorzShader.load( "shaders/onionBarsHorz" );
//	shaderMap["onionBarsHorzShader"] = &onionBarsHorzShader;
//	
//	onionDotsShader.load( "shaders/onionDots" );
//	shaderMap["onionDotsShader"] = &onionDotsShader;
	
	//store the shader names in a vecotr for use in a radio gui
	for( map<string, ofShader*>::iterator it = shaderMap.begin(); it!= shaderMap.end(); it++){
		shaderNames.push_back(it->first);
	}
	
//	currentShader = &normalShader;
}


//--------------------------------------------------------------
void ofApp::update()
{
    ofSetWindowTitle(ofToString(ofGetFrameRate())+"fps");
    
	//preset load/save
	if(bSavePreset){
		savePreset();
	}
    
	if(currentPresetName != nextPreset){
		if(!bCachedPresetValues){
			bCachedPresetValues = true;
			cachePresetValues();
			
			sideviewValues = presets["sideView"];
//			for (map<string, float>::iterator it = sideviewValues.begin(); it!=sideviewValues.end(); it++) {
//				cout << it->first << " : " << it->second << endl;
//			}
			
		}
		loadPreset( nextPreset );
		
		currentValues = presets[ nextPreset ];
		lastValues = presets[ nextPreset ];
	}
	
    
    
    if(!bAddingRibbon && queue.size() > 0)
    {
		ofLogNotice() << "adding journey from queue";
        
		//true for animating in
        journeys.push_back( queue[0] );
        queue.erase(queue.begin());
        
		bJourniesNeedUpdate = true;
        
		addJourneyTween();
    }
    
    
	//add new journeys
	if(bJourniesNeedUpdate){
		bJourniesNeedUpdate = false;
		
		onions.resize( journeys.size() );
		
		for(int i=0; i<onions.size(); i++)
		{
			if(!onions[i].bIsSetup){
				onions[i].setup( journeys[i] );
				onions[i].color = getNextJourneyColor();//getRandomColor();
				
				//prevent neighbor color matches
				if(i>0){
					while (onions[i].color == onions[i-1].color) {
						onions[i].color = getNextJourneyColor();// getRandomColor();
					}
				}
			}
		}
	}
	
	//animation
	float currentTime = ofGetElapsedTimef();
	timedelta = currentTime - lasttime;
	lasttime = currentTime;
	elapsedTime += timedelta * timeScale;
	
	//tweens auto update vi ofListener
    Eul.set( 0, pow(sin(elapsedTime * .8), 3.)*3, pow(sin(elapsedTime * .4), 3.)*10. );
	Eul *= EulScale;
    
    soundManager.rotVel.set( Eul );
    

    
    if(!bClientConnected && currentTime-lastConnectionAttempt > 5) {
        client.connect( options );
        lastConnectionAttempt = currentTime;
    }
}

void ofApp::retryColors()
{
	for(int i=0; i<onions.size(); i++){
		onions[i].color = getNextJourneyColor();//getRandomColor();
	}
}

//--------------------------------------------------------------
void ofApp::draw()
{
	
	camera.reset();
	
	ofBackground( 0, 0, 0, 255);
	ofPushStyle();
	
	fbo.begin();
	ofClear(0,0,0,255);
		
	if (bDepthTest) {
		glEnable( GL_DEPTH_TEST);
	}else{
		glDisable(GL_DEPTH_TEST);
	}

	drawOnion();
	
	if (!bDepthTest) {
		glDisable(GL_DEPTH_TEST);
	}
	
	fbo.end();
	
	ofPopStyle();
	
	ofPushStyle();
	
	sideViewFbo.begin();
	ofClear(0,0,0,255);
		
	if (bDepthTest) {
		glEnable( GL_DEPTH_TEST);
	}else{
		glDisable(GL_DEPTH_TEST);
	}
	
	drawSideView();
	
	if (!bDepthTest) {
		glDisable(GL_DEPTH_TEST);
	}
	
	sideViewFbo.end();
	
	//mip your own maps
	drawMipMaps();
	
	
	ofPopStyle();
	ofSetColor(255,255,255,255);
	ofPushStyle();
	ofEnableAlphaBlending();
	
	//post shader. draw to the screen
	post.begin();
	post.setUniform2f("center", 3 * ofGetWidth()/4 + screenPosOffsetX, ofGetHeight()/2 - screenPosOffsetY);
	post.setUniform1f("circleRadius", circleRadius );
	post.setUniform1f("edgeAADist", edgeAADist );
	post.setUniformTexture("fbo", fbo.getTextureReference(), 0);
	post.setUniformTexture("mm1", fbo_mm1.getTextureReference(), 1);
	post.setUniformTexture("mm2", fbo_mm2.getTextureReference(), 2);
	post.setUniformTexture("mm3", fbo_mm3.getTextureReference(), 3);
	post.setUniformTexture("mm4", fbo_mm4.getTextureReference(), 4);
	post.setUniformTexture("mm5", fbo_mm5.getTextureReference(), 5);
	post.setUniformTexture("mm6", fbo_mm6.getTextureReference(), 6);
	post.setUniform1f("glowCoefficient", glowCoefficient );
	post.setUniform1f("glowExponent", glowExponent );
	post.setUniform1f("glowScale", glowScale );
	
	fbo.draw( ofGetWidth()/2 + screenPosOffsetX, screenPosOffsetY, fbo.getWidth(), ofGetHeight() );
	
	post.end();
	ofPopStyle();
	ofPopStyle();
	ofSetColor(255,255,255,255);
	ofPushStyle();
	ofEnableAlphaBlending();
	
	//post shader. draw to the screen
	glDisable(GL_DEPTH_TEST);
	
	post.begin();
	post.setUniform1f("circleRadius", 10000 );// circleRadius );
	post.setUniform1f("edgeAADist", edgeAADist );
	post.setUniformTexture("fbo", sideViewFbo.getTextureReference(), 0);
	post.setUniformTexture("mm1", sideView_mm1.getTextureReference(), 1);
	post.setUniformTexture("mm2", sideView_mm2.getTextureReference(), 2);
	post.setUniformTexture("mm3", sideView_mm3.getTextureReference(), 3);
	post.setUniformTexture("mm4", sideView_mm4.getTextureReference(), 4);
	post.setUniformTexture("mm5", sideView_mm5.getTextureReference(), 5);
	post.setUniformTexture("mm6", sideView_mm6.getTextureReference(), 6);
	post.setUniform1f("glowCoefficient", glowCoefficient );
	post.setUniform1f("glowExponent", glowExponent );
	post.setUniform1f("glowScale", glowScale );
	
	sideViewFbo.draw( 0, 0, sideViewFbo.getWidth(), ofGetHeight() );
	
	post.end();
	ofPopStyle();
    
    if(bDebugScreen){
//        ofBackground( 255,255,255,255);
        
        ofPushStyle();
        
        //post shader. draw to the screen
        glDisable(GL_DEPTH_TEST);
        
        post.begin();
        post.setUniform2f("center", 3 * ofGetWidth()/4 + screenPosOffsetX, ofGetHeight()/2 - screenPosOffsetY);
        post.setUniform1f("circleRadius", circleRadius );
        post.setUniform1f("edgeAADist", edgeAADist );
        post.setUniform1f("edgeAADist", edgeAADist );
        post.setUniformTexture("fbo", debubgFill.getTextureReference(), 0);
        post.setUniformTexture("mm1", debubgFill.getTextureReference(), 1);
        post.setUniformTexture("mm2", debubgFill.getTextureReference(), 2);
        post.setUniformTexture("mm3", debubgFill.getTextureReference(), 3);
        post.setUniformTexture("mm4", debubgFill.getTextureReference(), 4);
        post.setUniformTexture("mm5", debubgFill.getTextureReference(), 5);
        post.setUniformTexture("mm6", debubgFill.getTextureReference(), 6);
        
        debubgFill.draw( ofGetWidth()/2 + screenPosOffsetX, screenPosOffsetY, fbo.getWidth(), ofGetHeight() );
        
        post.end();
        
        ofSetColor(255, 255, 255);
        for(int i=0; i <= ofGetWidth()/100; i++){
            ofLine( i*100, 0, i*100, ofGetHeight() );
        }
        
        for(int i=0; i <= ofGetHeight()/100; i++){
            ofLine( 0, i*100, ofGetWidth(), i*100 );
        }
        
        ofPopStyle();
    }
	
	
	//draw mip maps if gui is visible 
	if(guis[0]->isVisible()) {
		
		ofDisableAlphaBlending();
		glDisable( GL_DEPTH_TEST );
		ofSetColor(255,255,255,255);
		fbo_mm1.draw(ofGetWidth() - 110, 10, 100, 100 );
		fbo_mm2.draw(ofGetWidth() - 110, 110, 100, 100 );
		fbo_mm3.draw(ofGetWidth() - 110, 220, 100, 100 );
		fbo_mm4.draw(ofGetWidth() - 110, 330, 100, 100 );
		fbo_mm5.draw(ofGetWidth() - 110, 440, 100, 100 );
		fbo_mm6.draw(ofGetWidth() - 110, 550, 100, 100 );
		
		
		for (int i=0; i<onions.size(); i++) {
			onions[i].dataTexture.draw( ofGetWidth() - ofGetWidth()/6 - 5, ofGetHeight() - 10 - 5*i, ofGetWidth()/6, 4.5 );
		}
		
		glEnable( GL_DEPTH_TEST );
		ofEnableAlphaBlending();
	}
}

void ofApp::drawOnion()
{
	if(!bOnionSetup){
		setupOnion();
	}
	
	//update onion transforms
	ofQuaternion q;
	q.makeRotate(Eul.x, ofVec3f(1,0,0), Eul.y, ofVec3f(0,1,0), Eul.z, ofVec3f(0,0,1));
	
	float startScale = 1. / recursiveScale;
	
	minSampleVal = 10000000;
	for (int i=onions.size()-1; i>=0; i--)
	{
		if(i == onions.size()-1 )
		{
			onions[i].transform.setScale( newRibbonScale,newRibbonScale,newRibbonScale ); // newRibbonScale scales down to 1.
			onions[i].transform.setOrientation( q + q.inverse() * (1.f - newRibbonShaderScale) ); // newRibbonShaderScale == val btwn 0-1
			onions[i].sampleVal = newRibbonScale;
		}
		else
		{
			float scl = onions[i+1].transform.getScale().x * recursiveScale;
			scl -= floor(scl);
			onions[i].transform.setScale( scl, scl, scl );
			onions[i].transform.setOrientation( onions[i+1].transform.getOrientationQuat() * q );
			onions[i].sampleVal = scl;//onions[i+1].transform.getScale().x * recursiveScale;
			minSampleVal = min( minSampleVal, onions[i].sampleVal );
			
			
			if(onions[i+1].transform.getScale().x > 1.01 ){
				onions[i+1].transform.setScale( 1.01, 1.01, 1.01 );
			}
		}
	}
	
	globalTransform.makeIdentityMatrix();
	globalTransform.rotateRad(globalRotationAboutXAxis, 1, 0, 0);
	
	//draw it
	camera.begin();
	
	//global onion uniforms
	currentShader->begin();
	currentShader->setUniform1f("time", elapsedTime );
	currentShader->setUniform1f("dataSmoothing", dataSmoothing);
	currentShader->setUniform1f("noiseScale", noiseScale );
	currentShader->setUniform1f("slope", slope );
	
	currentShader->setUniform1f("noiseExponent", noiseExponent );
	currentShader->setUniform1f("noiseMixExponent", noiseMixExponent );
	currentShader->setUniform1f("noisePosScale", noisePosScale );
	currentShader->setUniform1f("noiseSpread", noiseSpread );
	
	currentShader->setUniform1f("tunnelMix", tunnelMix );
	currentShader->setUniform1f("tunnelDeltaScl", tunnelDeltaScl );
	currentShader->setUniform1f("tunnelTimeScl", tunnelTimeScl );
	currentShader->setUniform1f("tunnelDepthScl", tunnelDepthScl );
	
	//
	ofPushMatrix();
	
	ofScale( radius, radius, radius );
	ofTranslate( onionPosX, onionPosY, onionPosZ );
	
	ofTranslate( positionX, positionY, positionZ );
	
	ofRotateX( rotateX );
	ofRotateY( rotateY );
	ofRotateZ( rotateZ );
	
	ofPushMatrix();
	
	if( bRotateOnNewJourney )	ofMultMatrix( globalTransform );//<-- this rotates the onion on transition in
	
	ofScale(1, squishY, squish );
	
	ofRotate( newRibbonShaderScale * -360 + 90 - slope*90., 0, 0, 1);
	float vortexRotVal = -3. * elapsedTime * vortexScl;
	
	
	if(!bDepthTest)	glDisable( GL_DEPTH_TEST );
	
	glEnable(GL_CULL_FACE);
	
	
	//draw each onion
	float step = 1. / float(onions.size()-1);
	for (int i=0; i<onions.size(); i++) {
		
		//crazy transforms
		ofPushMatrix();
		ofMultMatrix( onions[i].transform.getGlobalTransformMatrix() );
		
		//vortex rotation
		ofRotate( ( max( (float) onions.size() - i - 2 + newRibbonShaderScale, 0.f) ) * vortexRotVal, 0, 0, 1);
		
		//set ribbon color
		ofSetColor( onions[i].color );
		
		//per ribbon uniforms
		float sampleVal = ofMap( onions[i].sampleVal, minSampleVal, 1., 0, 1, true );
		ofFloatColor col = getColor( sampleVal, &controlColors );
		
		float mixVal = ofMap(sampleVal, 0, 1, journeyColorMixLow, journeyColorMixHi, true);
		
		col.r = ofMap(mixVal, 0, 1, col.r, float(onions[i].color.r)/255., true );
		col.g = ofMap(mixVal, 0, 1, col.g, float(onions[i].color.g)/255., true );
		col.b = ofMap(mixVal, 0, 1, col.b, float(onions[i].color.b)/255., true );
		
		currentShader->setUniform1f( "sampleVal", sampleVal );
		currentShader->setUniform1f( "randomOffset", onions[i].randomNumer );
		currentShader->setUniform1f( "displacement", ofMap( onions[i].sampleVal, minSampleVal, 1., innerDisplacement, outerDisplacement, true ) );
		currentShader->setUniform1f( "readingScale", ofMap( onions[i].sampleVal, minSampleVal, 1., innerReadingScale, outerReadingScale, true ) );
		currentShader->setUniform1f( "readingThreshold", ofMap( onions[i].sampleVal, minSampleVal, 1., innerReadingThreshold, outerReadingThreshold, true ) );
		currentShader->setUniform1f( "alpha", ofMap( onions[i].sampleVal, minSampleVal, 1., innerAlpha, outerAlpha, true ) );
		currentShader->setUniform1f( "facingRatio", ofMap( onions[i].sampleVal, minSampleVal, 1., innerFacingRatio, outerFacingRatio, true ) );
		currentShader->setUniform2f( "texDim", onions[i].dataTexture.getWidth(), onions[i].dataTexture.getHeight() );
		currentShader->setUniform3f( "blendColor", col.r, col.g, col.b );
		currentShader->setUniformTexture( "dataTexture", onions[i].dataTexture, 0);
		
		//we only animimate the outer onion
		if( bAddingRibbon && i == onions.size()-1 )
		{
			currentShader->setUniform1f("animateIn", newRibbonShaderScale );
			
//			// after .95 it fades to the normal preset color
//			float mixVal = ofMap( newRibbonShaderScale, .95, 1, 0, 1, true );
//			float mMixVal = 1. - mixVal;
//			
//			currentShader->setUniform3f( "blendColor", mMixVal*journeyIntroColor.r+mixVal*col.r, mMixVal*journeyIntroColor.g+mixVal*col.g, mMixVal*journeyIntroColor.b+mixVal*col.b );
		}
		else
		{
			//1 == no animation.
			currentShader->setUniform1f("animateIn", 1 );
		}
		
		//draw front and back in diferent passes
		glCullFace(GL_FRONT);
		sphereVbo.drawElements( GL_TRIANGLES, spherVboIndexCount );
		
		glCullFace(GL_BACK);
		sphereVbo.drawElements( GL_TRIANGLES, spherVboIndexCount );
		
		ofPopMatrix();
	}
	
	
	glDisable(GL_CULL_FACE);
	
	currentShader->end();
	
	//	//AMBIENT PARTICLES
	//	glPointSize( 10 );
	//	glEnable( GL_VERTEX_PROGRAM_POINT_SIZE );
	//
	//	ofSetColor(255, 0, 0);
	//	particleVbo.draw( GL_POINTS, 0, numParticles );
	
	ofPopMatrix();
	
	ofPopMatrix();
	
	
	if(!bDepthTest)	glEnable( GL_DEPTH_TEST );
	camera.end();
}

void ofApp::drawSideView()
{
	
	ofQuaternion q;
	q.makeRotate(Eul.x, ofVec3f(1,0,0), Eul.y, ofVec3f(0,1,0), Eul.z, ofVec3f(0,0,1));
	
	float startScale = 1. / recursiveScale;
	
	minSampleVal = 10000000;
	for (int i=onions.size()-1; i>=0; i--)
	{
		if(i == onions.size()-1 )
		{
			onions[i].transform.setScale( newRibbonScale,newRibbonScale,newRibbonScale ); // newRibbonScale scales down to 1.
			onions[i].transform.setOrientation( q + q.inverse() * (1.f - newRibbonShaderScale) ); // newRibbonShaderScale == val btwn 0-1
			onions[i].sampleVal = newRibbonScale;
		}
		else
		{
			float scl = onions[i+1].transform.getScale().x * recursiveScale;
			scl -= floor(scl);
			onions[i].transform.setScale( scl, scl, scl );
			onions[i].transform.setOrientation( onions[i+1].transform.getOrientationQuat() * q );
			onions[i].sampleVal = scl;//onions[i+1].transform.getScale().x * recursiveScale;
			minSampleVal = min( minSampleVal, onions[i].sampleVal );
		}
	}
	
	globalTransform.makeIdentityMatrix();
	globalTransform.rotateRad(globalRotationAboutXAxis, 1, 0, 0);
	
	
	//draw it
	camera.begin();
	
	//global onion uniforms
	currentShader->begin();
	currentShader->setUniform1f("time", elapsedTime );
	
	currentShader->setUniform1f("dataSmoothing", sideviewValues["dataSmoothing"]);
	currentShader->setUniform1f("noiseScale", sideviewValues["noiseScale"] );
	currentShader->setUniform1f("slope", sideviewValues["slope"] );
	
	currentShader->setUniform1f("dataSmoothing", sideviewValues["dataSmoothing"]);
	currentShader->setUniform1f("noiseScale", sideviewValues["noiseScale"] );
	currentShader->setUniform1f("slope", sideviewValues["slope"] );
	
	currentShader->setUniform1f("noiseExponent", sideviewValues["noiseExponent"] );
	currentShader->setUniform1f("noiseMixExponent", sideviewValues["noiseMixExponent"] );
	currentShader->setUniform1f("noisePosScale", sideviewValues["noisePosScale"] );
	currentShader->setUniform1f("noiseSpread", sideviewValues["noiseSpread"] );
	
	currentShader->setUniform1f("tunnelMix", sideviewValues["tunnelMix"] );
	currentShader->setUniform1f("tunnelDeltaScl", sideviewValues["tunnelDeltaScl"] );
	currentShader->setUniform1f("tunnelTimeScl", sideviewValues["tunnelTimeScl"] );

	currentShader->setUniform1f("tunnelDepthScl", sideviewValues["tunnelDepthScl"] );
	
	//
	ofPushMatrix();
	
	ofScale( sideviewValues["radius"], sideviewValues["radius"], sideviewValues["radius"] );
	ofTranslate( sideviewValues["onionPosX"], sideviewValues["onionPosY"], sideviewValues["onionPosZ"] );
	ofTranslate( sideviewValues["positionX"], sideviewValues["positionY"] + cameraOffsetVal, sideviewValues["positionZ"] );

	ofRotateX( sideviewValues["rotateX"] );
	ofRotateY( sideviewValues["rotateY"] );
	ofRotateZ( sideviewValues["rotateZ"] );
	
	ofPushMatrix();
	
	if( bRotateOnNewJourney )	ofMultMatrix( globalTransform );//<-- this rotates the onion on transition in
	
	ofScale(1, sideviewValues["squishY"], sideviewValues["squish"] );
	
	ofRotate( newRibbonShaderScale * -360 + 90 - slope*90., 0, 0, 1);
	float vortexRotVal = -3. * elapsedTime * vortexScl;
	
	
	if(!bDepthTest)	glDisable( GL_DEPTH_TEST );
	glEnable( GL_DEPTH_TEST );
	
	glEnable(GL_CULL_FACE);
	
	
	//draw each onion
	float step = 1. / float(onions.size()-1);
	for (int i=onions.size()-8; i<onions.size(); i++) {
		
		//crazy transforms
		ofPushMatrix();
		ofMultMatrix( onions[i].transform.getGlobalTransformMatrix() );
		
		//vortex rotation
		ofRotate( ( max( (float) onions.size() - i - 2 + newRibbonShaderScale, 0.f) ) * vortexRotVal, 0, 0, 1);
		
		//set ribbon color
		ofSetColor( onions[i].color );
		
		//per ribbon uniforms
		float sampleVal = ofMap( onions[i].sampleVal, minSampleVal, 1., 0, 1, true );
		ofFloatColor col = getColor( sampleVal, &controlColors );
		
		float mixVal = ofMap(sampleVal, 0, 1, journeyColorMixLow, journeyColorMixHi, true);
		
		col.r = ofMap(mixVal, 0, 1, col.r, float(onions[i].color.r)/255., true );
		col.g = ofMap(mixVal, 0, 1, col.g, float(onions[i].color.g)/255., true );
		col.b = ofMap(mixVal, 0, 1, col.b, float(onions[i].color.b)/255., true );
		
		currentShader->setUniform1f( "sampleVal", sampleVal );
		currentShader->setUniform1f( "randomOffset", onions[i].randomNumer );
		currentShader->setUniform1f( "displacement", ofMap( onions[i].sampleVal, minSampleVal, 1., sideviewValues["innerDisplacement"], sideviewValues["outerDisplacement"], true ) );
		currentShader->setUniform1f( "readingScale", ofMap( onions[i].sampleVal, minSampleVal, 1., sideviewValues["innerReadingScale"], sideviewValues["outerReadingScale"], true ) );
		currentShader->setUniform1f( "readingThreshold", ofMap( onions[i].sampleVal, minSampleVal, 1., sideviewValues["innerReadingThreshold"], sideviewValues["outerReadingThreshold"], true ) );
		currentShader->setUniform1f( "alpha", ofMap( onions[i].sampleVal, minSampleVal, 1., sideviewValues["innerAlpha"], sideviewValues["outerAlpha"], true ) );
		currentShader->setUniform1f( "facingRatio", ofMap( onions[i].sampleVal, minSampleVal, 1., sideviewValues["innerFacingRatio"], sideviewValues["outerFacingRatio"], true ) );
		currentShader->setUniform2f( "texDim", onions[i].dataTexture.getWidth(), onions[i].dataTexture.getHeight() );
		currentShader->setUniform3f( "blendColor", col.r, col.g, col.b );
		currentShader->setUniformTexture( "dataTexture", onions[i].dataTexture, 0);
		
		//we only animimate the outer onion
		if( bAddingRibbon && i == onions.size()-1 )
		{
			currentShader->setUniform1f("animateIn", newRibbonShaderScale );
			
//			// after .95 it fades to the normal preset color
//			float mixVal = ofMap( newRibbonShaderScale, .95, 1, 0, 1, true );
//			float mMixVal = 1. - mixVal;
//			
//			currentShader->setUniform3f( "blendColor", mMixVal*journeyIntroColor.r+mixVal*col.r, mMixVal*journeyIntroColor.g+mixVal*col.g, mMixVal*journeyIntroColor.b+mixVal*col.b );

		}
		else
		{
			//1 == no animation.
			currentShader->setUniform1f("animateIn", 1 );
		}
		
		//draw front and back in diferent passes
		
		
		glCullFace(GL_FRONT);
		sphereVbo.drawElements( GL_TRIANGLES, spherVboIndexCount );
		
		glCullFace(GL_BACK);
		sphereVbo.drawElements( GL_TRIANGLES, spherVboIndexCount );
		
		
		ofPopMatrix();
		
		
	}
	
	
	
	glDisable(GL_CULL_FACE);
	
	currentShader->end();
	
	
	ofPopMatrix();
	
	ofPopMatrix();
	
	
	//AMBIENT PARTICLES
	glDisable( GL_DEPTH_TEST );
	glEnable( GL_VERTEX_PROGRAM_POINT_SIZE );
	ofEnableAlphaBlending();
	
	float noiseTime = elapsedTime * .1;
	ofVec3f noiseVec( ofNoise(noiseTime), ofNoise(noiseTime + 2), ofNoise(noiseTime + 3)  );
	noiseVec *= 10.;
	
	ofPushMatrix();
	ofRotate( elapsedTime * .3, 0, 1, 0 );
	//	ofScale( sideviewValues["radius"], sideviewValues["radius"], sideviewValues["radius"] );
	ofTranslate( sideviewValues["positionX"] + noiseVec.x, sideviewValues["positionY"] + cameraOffsetVal * sideviewValues["radius"] + noiseVec.y, sideviewValues["positionZ"] + noiseVec.z);
	//	ofMultMatrix( onions.back().transform.getGlobalTransformMatrix() );
	
	
	particleShader.begin();
	particleShader.setUniform1f( "time", elapsedTime );
	ofSetColor(255, 0, 0);
	particleVbo.draw( GL_POINTS, 0, numParticles );
	particleShader.end();
	
	ofPopMatrix();
	
	
	if(!bDepthTest)	glEnable( GL_DEPTH_TEST );
	camera.end();
	
}

void ofApp::drawMipMaps(){
	
	ofSetColor(255,255,255,255);
	
	fbo_mm1.begin();
	ofClear(0,0,0,255);
	fbo.draw(0, 0, fbo_mm1.getWidth(), fbo_mm1.getHeight() );
	fbo_mm1.end();
	
	fbo_mm2.begin();
	ofClear(0,0,0,255);
	fbo_mm1.draw(0, 0, fbo_mm2.getWidth(), fbo_mm2.getHeight() );
	fbo_mm2.end();
	
	fbo_mm3.begin();
	ofClear(0,0,0,255);
	fbo_mm2.draw(0, 0, fbo_mm3.getWidth(), fbo_mm3.getHeight() );
	fbo_mm3.end();
	
	fbo_mm4.begin();
	ofClear(0,0,0,255);
	fbo_mm3.draw(0, 0, fbo_mm4.getWidth(), fbo_mm4.getHeight() );
	fbo_mm4.end();
	
	fbo_mm5.begin();
	ofClear(0,0,0,255);
	fbo_mm4.draw(0, 0, fbo_mm5.getWidth(), fbo_mm5.getHeight() );
	fbo_mm5.end();
	
	fbo_mm6.begin();
	ofClear(0,0,0,255);
	fbo_mm5.draw(0, 0, fbo_mm6.getWidth(), fbo_mm6.getHeight() );
	fbo_mm6.end();
	
	
	
	sideView_mm1.begin();
	ofClear(0,0,0,255);
	sideViewFbo.draw(0, 0, sideView_mm1.getWidth(), sideView_mm1.getHeight() );
	sideView_mm1.end();
	
	sideView_mm2.begin();
	ofClear(0,0,0,255);
	sideView_mm1.draw(0, 0, sideView_mm2.getWidth(), sideView_mm2.getHeight() );
	sideView_mm2.end();
	
	sideView_mm3.begin();
	ofClear(0,0,0,255);
	sideView_mm2.draw(0, 0, sideView_mm3.getWidth(), sideView_mm3.getHeight() );
	sideView_mm3.end();
	
	sideView_mm4.begin();
	ofClear(0,0,0,255);
	sideView_mm3.draw(0, 0, sideView_mm4.getWidth(), sideView_mm4.getHeight() );
	sideView_mm4.end();
	
	sideView_mm5.begin();
	ofClear(0,0,0,255);
	sideView_mm4.draw(0, 0, sideView_mm5.getWidth(), sideView_mm5.getHeight() );
	sideView_mm5.end();
	
	sideView_mm6.begin();
	ofClear(0,0,0,255);
	sideView_mm5.draw(0, 0, sideView_mm6.getWidth(), sideView_mm6.getHeight() );
	sideView_mm6.end();

}

//--------------------------------------------------------------
void ofApp::setupSphere( float radians, float sphereRad )
{
	//create mesh tube
	int subdX = 64, subdY = 64;//128*128 ~= 16000 * (pos+norm+tangent+bitangent+uv+indices) == a lot of data
	
	int numVertices = subdX * subdY;
	vector< ofVec3f >vertices( numVertices );
	vector< ofVec3f >normals( numVertices );
	vector< ofVec3f >tangents( numVertices );
	vector< ofVec3f >binormals( numVertices );
	vector< ofVec2f >texCoords( numVertices );
	
	//make our vertices
	int count = 0;
	float xStep = 1. / float(subdX-1);
	float yStep = 1. / float(subdY-1);
	float zStep = -subdY / 2;
	
	float xval, yval;
	for (int i=0; i<subdY; i++) {
		
		//make some vertices
		for (int j=0; j<subdX; j++) {
			
			xval = xStep*j*TWO_PI;
			yval = yStep*i * radians;
			float u = xval;
			float v = yval;
			
			normals[count].set( cos(u)*sin(v), sin(u)*sin(v), -cos(v) );
			vertices[count] = normals[count] * sphereRad;
			texCoords[count].set( j * xStep, i * yStep );
			
			count++;
		}
	}
	
	//make the faces, calculate the face normals and add them to the vertex normals
	vector<ofIndexType> indices;
	ofVec3f faceNorm;
	int p0, p1, p2, p3, wrapIndex;
	for (int i=1; i<subdY; i++) {
		for (int j=1; j<subdX; j++) {
			
			//triangle faces
			p0 = (i-1) * subdX + j-1;
			p1 = i*subdX + j-1;
			p2 = i*subdX + j;
			p3 = (i-1) * subdX + j;
			
			indices.push_back( p0 );
			indices.push_back( p1 );
			indices.push_back( p2 );
			
			indices.push_back( p0 );
			indices.push_back( p2 );
			indices.push_back( p3 );
		}
	}
	spherVboIndexCount = indices.size();
	
	//compute tangents and bi-normals
	vector<ofVec3f> tan1( vertices.size(), ofVec3f(0) );
	vector<ofVec3f> tan2( vertices.size(), ofVec3f(0) );
	ofVec3f sdir, tdir;
	ofVec4f tangent;
	int i1, i2, i3;
	float x1, x2, y1, y2, z1, z2, s1, s2, t1, t2, r;
	
	for(int f=0; f<indices.size(); f+=3){
		i1 = indices[f];
		i2 = indices[f+1];
		i3 = indices[f+2];
		
		ofVec3f& v1 = vertices[i1];
		ofVec3f& v2 = vertices[i2];
		ofVec3f& v3 = vertices[i3];
		
		ofVec2f& w1 = texCoords[i1];
		ofVec2f& w2 = texCoords[i2];
		ofVec2f& w3 = texCoords[i3];
		
		
		x1 = v2.x - v1.x;
		x2 = v3.x - v1.x;
		y1 = v2.y - v1.y;
		y2 = v3.y - v1.y;
		z1 = v2.z - v1.z;
		z2 = v3.z - v1.z;
		
		s1 = w2.x - w1.x;
		s2 = w3.x - w1.x;
		t1 = w2.y - w1.y;
		t2 = w3.y - w1.y;
		
		r = 1.0F / (s1 * t2 - s2 * t1);
		sdir.set((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
				 (t2 * z1 - t1 * z2) * r);
		tdir.set((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
				 (s1 * z2 - s2 * z1) * r);
		
		tan1[i1] += sdir;
		tan1[i2] += sdir;
		tan1[i3] += sdir;
		
		tan2[i1] += tdir;
		tan2[i2] += tdir;
		tan2[i3] += tdir;
	}
	
	for (int a=0; a < vertices.size(); a++){
		ofVec3f& n = normals[a];
		ofVec3f& t = tan1[a];
		
		// Gram-Schmidt orthogonalize
		tangent = (t - n * n.dot(t)).normalize();
		
		// Calculate handedness
		tangent.w = ( n.crossed(t).dot( tan2[a] ) < 0.) ? -1.f : 1.f;
		
		tangents[a] = tangent;
		binormals[a] = n.crossed( tangents[a] );
	}
	
	
	//add attributes to vbo
	sphereVbo.setVertexData( &vertices[0], vertices.size(), GL_STATIC_DRAW );
	sphereVbo.setNormalData( &normals[0], normals.size(), GL_STATIC_DRAW );
	sphereVbo.setTexCoordData( &texCoords[0], texCoords.size(), GL_STATIC_DRAW );
	sphereVbo.setIndexData( &indices[0], indices.size(), GL_STATIC_DRAW );
	
	displacedShader.begin();
	
	cout << displacedShader.getAttributeLocation( "tangent" ) << " : " << displacedShader.getAttributeLocation( "binormal" ) << endl;
	sphereVbo.setAttributeData( displacedShader.getAttributeLocation( "tangent" ), &tangents[0].x, 3, tangents.size()*3, GL_STATIC_DRAW, sizeof(tangents[0]) );
	sphereVbo.setAttributeData( displacedShader.getAttributeLocation( "binormal" ), &binormals[0].x, 3, binormals.size()*3, GL_STATIC_DRAW, sizeof(binormals[0])  );
	
	displacedShader.end();
	
}

void ofApp::setupOnion()
{
	bOnionSetup = true;
	setupSphere();
}

ofFloatColor ofApp::getColor(float sampleVal, vector<ofFloatColor> * _controlColors )
{
	float  sv = ofMap(sampleVal, 0, 1, 0, _controlColors->size()-1, true );
	
	int lowIndex = floor( sv );
	int hiIndex = ceil( sv );
	sv -= lowIndex;
	float msv = 1. - sv;
	
	ofFloatColor c0 = (*_controlColors)[lowIndex];
	ofFloatColor c1 = (*_controlColors)[hiIndex];
	
	
	return ofFloatColor( c0.r*msv + c1.r*sv, c0.g*msv + c1.g*sv, c0.b*msv + c1.b*sv, 1. );  ;//*.5 + controlColors[lowIndex]*5;
}

Reading ofApp::sampleJourney( int i, float u )
{
	Reading outVal;
	
	float sv = u * float(i);
	int low = floor( sv );
	int hi = ceil( sv );
	
	sv -= low;
	
	sv = tween.calcSinusoidal( sv, TWEEN_INOUT );
	
	outVal.setAttention( journeys[i]->readings[low].getAttention() * (1. - sv) + journeys[i]->readings[hi].getAttention() * sv );
	outVal.setMeditation( journeys[i]->readings[low].getMeditation() * (1. - sv) + journeys[i]->readings[hi].getMeditation() * sv );
	
	return outVal;
}

ofVec3f ofApp::normalFrom3Points(ofVec3f p0, ofVec3f p1, ofVec3f p2)
{
	ofVec3f norm = (p2 - p1).cross( p0 - p1);
	return norm.normalized();
}

ofVec3f ofApp::normalFrom4Points(ofVec3f p0, ofVec3f p1, ofVec3f p2, ofVec3f p3)
{
	return (normalFrom3Points(p0, p1, p2) + normalFrom3Points(p0, p2, p3)).normalized();
}


//--------------------------------------------------------------
void ofApp::exit()
{
	savePreset("Working");
//	cout << "saved preset working" << endl;
	
	if(bDebug)	cout << "removing listeners" << endl;
	for(int i=0; i<guis.size(); i++){
		
		ofRemoveListener(guis[i]->newGUIEvent,this,&ofApp::guiEvent);
		delete guis[i];
	}
	
	//delete our vbos
	if(bDebug)	cout << "deleting/clearing vbos" << endl;
	for (int i=vbos.size()-1; i>=0; i--) {
		vbos[i]->clear();
		delete vbos[i];
	}
	
	client.close();
    soundManager.exit();
    
	if(bDebug)	cout << "end of ofApp::exit" << endl;
	
	//???: we're getting this error every now and again on exit:
	//	libc++abi.dylib: terminate called throwing an exception
}

ofColor ofApp::getNextJourneyColor()
{
	ofColor outColor = colorArray[colorArrayIndex];
	colorArrayIndex++;
	if(colorArrayIndex >= colorArray.size())	colorArrayIndex = 0;
	return outColor;
}

//--------------------------------------------------------------
ofColor ofApp::getRandomColor()
{
	
	map<string, ofColor>::iterator it = colorMap.begin();
	advance( it, floor( ofRandom(0., colorMap.size() ) ) );
	return  it->second;
}

//--------------------------------------------------------------
vector<string> ofApp::getPresetNames()
{
	vector<string> presetNames;
	string path = "presets/";
	ofDirectory dir(path);
	dir.listDir();
	
	//go through and store our preset names
	for(int i = 0; i < dir.numFiles(); i++){
		presetNames.push_back(dir.getName(i));
	}
	
	return presetNames;
}

void ofApp::cachePresetValues()
{
	vector<string> presetNames = getPresetNames();
	
	for (int i=0; i<presetNames.size(); i++) {
		presets[ presetNames[i] ];
	}
	
	for (int i=0; i<presetNames.size(); i++)
	{
		for (int j=0; j<guis.size(); j++) {
			string fileName = "Presets/" + presetNames[i] + "/"+guis[j]->getName()+".xml";
			
			ofxXmlSettings xml;
			xml.load( fileName );
			for (int k=0; k<xml.getNumTags("Widget"); k++) {
				xml.pushTag("Widget", k);
				string name = xml.getValue("Name", "no_name");
				int kind = xml.getValue("Kind", 0);
					
				if( kind == 4 || kind == 5 ){
					presets[ presetNames[i] ][ name ] = xml.getValue("Value", 0.);
				}

				xml.popTag();
			}
			
		}
	}
	
//	//print the float values
//	for (map<string, map< string, float > >::iterator it = presets.begin(); it != presets.end(); it++) {
//		cout << endl << "preset name: "<< it->first << endl;
//		for (map<string, float>::iterator jt=it->second.begin(); jt!=it->second.end(); jt++) {
//			cout << it->first << " : " << jt->first << " : " << jt->second << endl;
//		}
//	}
	
}

void ofApp::loadPreset( string presetName)
{
	currentPresetName = presetName;
	
	for(int i = 0; i < guis.size(); i++)
    {
//		cout << "Presets/" + presetName + "/"+guis[i]->getName()+".xml" << endl;
		guis[i]->loadSettings( "Presets/" + presetName + "/"+guis[i]->getName()+".xml");
		presetRadio->activateToggle( presetName );
    }
}


void ofApp::savePreset( string presetName )
{
	bSavePreset = false;
	
    ofDirectory dir;
    string presetDirectory = ofToDataPath("Presets/" + presetName + "/");
    if(!dir.doesDirectoryExist(presetDirectory))
    {
        dir.createDirectory(presetDirectory);
		
        presetRadio->addToggle(presetGui->addToggle(presetName, true));
        presetGui->autoSizeToFitWidgets();
    }
	else{
		presetRadio->activateToggle( presetName );
	}
    
    for(int i = 0; i < guis.size(); i++)
    {
		cout << "saving: " << presetDirectory + guis[i]->getName()+".xml" << endl;
        guis[i]->saveSettings(presetDirectory + guis[i]->getName()+".xml");
    }
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	if( key == 'h' || key == 'H' ){
		for(vector<ofxUICanvas *>::iterator it = guis.begin(); it != guis.end(); ++it)
		{
			(*it)->toggleVisible();
		}
	}
    if( key == 'd' || key == 'D' ){
        bDebugScreen = !bDebugScreen;
    }
    
	if(key == 's' || key == 'S' ) {
		savePreset();
	}
	
	if(key == 'l' || key == 'L' ){
		loadShaders();
	}
	
	if(key == 'c'){
		retryColors();
	}
	
	if(key == 'f' || key == 'F'){
        //		captrure = true;
       ofToggleFullscreen();
	}
	
	if(key == 'e' || key == 'E'){
        //		captrure = true;
	}
	
	if(key == 'j' || key == ' '){
		if(!bAddingRibbon){
			
			//load a journey from file
			ofBuffer buffer = ofBufferFromFile("Journeys/journey_showJourney" + ofToString(int(ofRandom(4))) + ".json");
			if(buffer.size()){
				reader.parse( buffer.getText(), json );
				handleRoute( json );
			}
		}
	}
    
//    if( key == OF_KEY_LEFT ){
//        --screenPosOffsetX;
//        cout << "screenPosOffsetX = " << screenPosOffsetX << ";"<< endl << "screenPosOffsetY = " << screenPosOffsetY << ";"<< endl;
//    }
//    if( key == OF_KEY_RIGHT ){
//        ++screenPosOffsetX;
//        cout << "screenPosOffsetX = " << screenPosOffsetX << ";"<< endl << "screenPosOffsetY = " << screenPosOffsetY << ";"<< endl;
//    }
//    if( key == OF_KEY_UP ){
//        --screenPosOffsetY;
//        cout << "screenPosOffsetX = " << screenPosOffsetX << ";"<< endl << "screenPosOffsetY = " << screenPosOffsetY << ";"<< endl;
//    }
//    if( key == OF_KEY_DOWN ){
//        ++screenPosOffsetY;
//        cout << "screenPosOffsetX = " << screenPosOffsetX << ";"<< endl << "screenPosOffsetY = " << screenPosOffsetY << ";"<< endl;
//    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{
	
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y )
{
	
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{
	
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{
	
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{
//    screenSize = ofToString(w) + "x" + ofToString(h);
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg)
{
	
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo)
{
	
}

//--------------------------------------------------------------
void ofApp::onConnect( ofxLibwebsockets::Event& args )
{
    cout<<"on connected"<<endl;

}

//--------------------------------------------------------------
void ofApp::onOpen( ofxLibwebsockets::Event& args )
{
    cout<<"on open"<<endl;
    bClientConnected = true;
    
    if(!bClientInitialized) {
        client.send("{\"route\": \"initMe\"}");
        bClientInitialized = true;
    }
}

//--------------------------------------------------------------
void ofApp::onClose( ofxLibwebsockets::Event& args )
{
    cout<<"on close"<<endl;
    bClientConnected = false;
}

//--------------------------------------------------------------
void ofApp::onIdle( ofxLibwebsockets::Event& args )
{
    cout<<"on idle"<<endl;
}

//--------------------------------------------------------------
void ofApp::handleRoute( Json::Value& _json)
{
	string route = _json["route"].asString();

	if(bSaveJsonsToFile){
		
		string fileIndex = ofToString( journeys.size() );
		ofDirectory dir;
		string journeyDirectory = ofToDataPath("Journeys/");
		if(!dir.doesDirectoryExist(journeyDirectory))	dir.createDirectory(journeyDirectory);
		
//		cout << ( journeyDirectory + "journey_" + route + fileIndex  +".json").c_str() << endl;
		ofstream    fs( ( journeyDirectory + "journey_" + route + fileIndex+".json").c_str() );
		
		fs << _json << endl;
		
		fs.close();
	}
	
    
	// LB: I was getting crashes when I initialized the onion layers here so I moved that updae
    if(route=="showJourney") {
		// on start up to populate the animation
        //false for not animating in
        journeys.push_back(new Journey(json["journey"], false));
		bJourniesNeedUpdate = true;
    }
	
    else if(route=="addJourney" || route == "replayJourney") {
        ofLogNotice() << "ading journey to queue";
        queue.push_back( new Journey(json["journey"], true) );

    }
	
    else if(route=="removeJourney") {
        string uid = json["_id"].asString();
        for(int i=0; i<journeys.size(); i++) {
            if(journeys[i]->uid==uid) {
                journeys[i]->sayGoodbye();
            }
        }
    }
    else if(route=="tick") {
		
		//for testing, can go if iritating
        //ofLogVerbose() << "server time: " << iso8601toTimestamp(json["date"].asString());
    }
    else {
        ofLogWarning() << "Route " << route << " unknown..." << endl;
    }
	
}

//--------------------------------------------------------------
void ofApp::onMessage( ofxLibwebsockets::Event& args )
{
//cout<<"got message "<< args.message <<  endl;
	
    if ( !reader.parse( args.message, json ) ) {
        std::cout  << "Failed to parse json\n" << reader.getFormattedErrorMessages();
        return;
    }
    
	if(!bLoadJsonsFromFile)	handleRoute(json);
	
}

//--------------------------------------------------------------
void ofApp::onBroadcast( ofxLibwebsockets::Event& args )
{
    cout<<"got broadcast "<<args.message<<endl;
}


