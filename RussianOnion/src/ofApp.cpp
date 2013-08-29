#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetVerticalSync(true);
	
	bDebug = true;
		
	//fbo
//	fbo.allocate( ofGetWidth(), ofGetHeight(), GL_RGBA16F, 4 );
	
	ofFbo::Settings s;
	s.width         = ofGetWidth();
	s.height            = ofGetHeight();
	s.internalformat    = GL_RGBA;
	s.numColorbuffers   = 3;
	s.useDepth = true;
	s.numSamples = 0;
	fbo.allocate(s);
	
	//
    ofxLibwebsockets::ClientOptions options = ofxLibwebsockets::defaultClientOptions();
    options.host = "brainz.io"; //laserstorms-MacBook-Pro.local"; //
    options.port = 8080;
    bool connected = client.connect( options );
    
    client.addListener(this);
    ofSetFrameRate(60);
    
//    ofSetLogLevel(OF_LOG_VERBOSE);
	
	//Journy stuff
	bSaveJsonsToFile = false;//for debuggin it was faster to load them from file rather then wait for the server
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
		
//	fbo.allocate(1024*8, 1024*8);
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
	animationPresetTime = 10;
	animationPresetIndex0 = 0;
	animationPresetIndex1 = 1;
	animationPresets.push_back("d_0");
	animationPresets.push_back("d_1");
	animationPresets.push_back("d_2");
	animationPresets.push_back("d_3");
	animationPresets.push_back("d_4");
	animationPresets.push_back("d_5");
	animationPresets.push_back("d_6");
	animationPresets.push_back("d_7");
	animationPresets.push_back("d_8");
	animationPresets.push_back("d_9");
	bPlayAnimation = true;
	
	//kick off animation variation
	variationKey = tween.addTween( variation, 0, 1, ofGetElapsedTimef(), ofGetElapsedTimef(), "variation");
	variationTween = tween.getTween( variationKey );//<--a looping tween( basically a timer ) that triggers transitions between presets
	
	bAddingRibbon = false;
	newRibbonScale = 1;
	
	//add tween listener
	ofAddListener( TweenEvent::events, this, &ofApp::tweenEventHandler );
}


void ofApp::setDefaults(){
	camera.setFarClip(20000);
	camera.setNearClip(20);
	timeScale = 1.;
	radius = 10;
	nearDepthScale = 3000;
	farDepthScale = 3500;
	
	bDepthTest = true;
	
	slope = .05;
	bRotateOnNewJourney = false;
	newRibbonShaderScale = 1;
}


void ofApp::setupUI(){
	
	renderTypes.clear();
	renderTypes.push_back("onion");
	
    int columnWidth = 75;
	
	float dim = 24;
	float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
    float length = 320-xInit;
	
	setDefaults();
	
	ofxUICanvas* guiMain = new ofxUICanvas(columnWidth, 0, length+xInit, columnWidth);
	guiMain->setName("Main");
	guiMain->setPosition(10, 10);
	guiMain->setColorFill(ofxUIColor(200));
	guiMain->setColorFillHighlight(ofxUIColor(255));
	guiMain->setColorBack(ofxUIColor(20, 20, 20, 150));
	
	guiMain->addSpacer();
	guiMain->addToggle("playAnimation", &bPlayAnimation );
	guiMain->addLabel("render Types");
	guiMain->addRadio("renderTypes", renderTypes );
	
	guiMain->addLabel("main");
	guiMain->addSpacer();
	guiMain->addFPS();
	guiMain->addSpacer();
	guiMain->addSlider("timeScl", -5, 5, &timeScale );
	guiMain->addLabel("global rendering");
	guiMain->addToggle("depthTest", bDepthTest);

	guiMain->addSlider("radius", 1, 100, &radius );
	guiMain->addSlider("recursiveScale", .5, 1., &recursiveScale );
	guiMain->addSlider("squish", .01, 1., &squish );
	guiMain->addSlider("readingThreshold", 0., 1., &readingThreshold );
	guiMain->addSlider("readingScale", .01, 1., &readingScale );
	guiMain->addSlider("onionAlpha", .01, 1., &onionAlpha );
	guiMain->addSlider("dataSmoothing", .01, 1., &dataSmoothing );
	guiMain->addSlider("facingRatio", .01, 1., &facingRatio );
	guiMain->addSlider("displacement", -1000, 1000., &displacement );
	guiMain->addSlider("noiseScale", 0, .025, &noiseScale );
	guiMain->addSlider("slope", 0., .2, &slope );
	guiMain->addToggle("rotateOnNewJourney", &bRotateOnNewJourney );
	
	//create a radio for switching renderTypes
	guiMain->addSpacer();
	guiMain->addLabel("Shaders" );
	cout << "shaderNames.size(): "<< shaderNames.size() << endl;
	guiMain->addRadio("shaders", shaderNames );
	
	
	guiMain->autoSizeToFitWidgets();
	
	
	
	//get our presets and them to the radio
	presetGui = new ofxUICanvas();
	
	//save button
	presetGui->addSpacer();
	presetGui->addButton("save preset", false);
	presetGui->addSpacer();
	
	presetGui->setName("PRESETS");
	presetGui->addLabel("Presets");
	presetGui->setPosition( guiMain->getRect()->getX() + guiMain->getRect()->getWidth() + 20, guiMain->getRect()->getY() );
	presetGui->addSpacer();
	presetRadio = presetGui->addRadio("presets", getPresetNames(), false );
	
	presetGui->autoSizeToFitWidgets();
	
	
	//add listeners
	guis.push_back( guiMain  );
	guis.push_back( presetGui );
	
	ofAddListener( guiMain->newGUIEvent,this,&ofApp::guiEvent );
	ofAddListener( presetGui->newGUIEvent,this,&ofApp::guiEvent );
	
	
	//load our working sttings
	//	loadPreset("Working");
	nextPreset = "Working";
	bCachedPresetValues = false;
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
						
						currentRenderType = name;
						
						cout << "currentRenderType: " << currentRenderType << endl;
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
	
	float newRibbonScaleDuration = 10;
	
	float startScale = 1. / recursiveScale;
	
	//action when a new ribbon animates in
	if(e.name == addRibbonTween)
	{
		if( e.message == "started" )
		{
			if(!bAddingRibbon)
			{
				bAddingRibbon = true;
				tween.addTween(globalRotationAboutXAxis, globalRotationAboutXAxis, globalRotationAboutXAxis+HALF_PI, ofGetElapsedTimef(), newRibbonAnimationSpan, "globalRotX", TWEEN_SINUSOIDAL, TWEEN_INOUT );
				
				tween.addTween(newRibbonScale, startScale, 1, ofGetElapsedTimef(), newRibbonScaleDuration, "newRibbonScale", TWEEN_SINUSOIDAL, TWEEN_INOUT );
				newRibbonShaderScale = 0;
			}
		}
	}
	
	//if we're adding a journey && it's rotated down then we create a tween to animate in the new ribbon
	if( bAddingRibbon && e.name == "globalRotX" && e.message == "ended" )
	{
		addRibbonScaleTween = tween.addTween( newRibbonShaderScale, 0, 1, ofGetElapsedTimef(), newRibbonScaleDuration, "newRibbonShaderScale", TWEEN_SINUSOIDAL, TWEEN_INOUT );
	}
	
	//if the ribbon is done scalling in rotate it back and remove any old journeys
	if(e.name == addRibbonScaleTween && e.message == "ended"){
		
		//TODO: magic number
		//remove the old journeys & onions
		if(onions.size() > 30){ // could be while() here?
			onions.erase( onions.begin() );
			journeys.erase( journeys.begin() );
		}
		
		rotatedBack = tween.addTween(globalRotationAboutXAxis, globalRotationAboutXAxis, 0, ofGetElapsedTimef(), newRibbonAnimationSpan, "rotatedBack", TWEEN_SINUSOIDAL, TWEEN_INOUT );
	}
	
	//end the adding new ribbon transition
	if(e.name == rotatedBack && e.message == "ended" )
	{
		bAddingRibbon = false;
	}
	
	
	
	//animation variation over time -> blending presets.
	if(e.name == variationKey)
	{
		if( e.message == "ended")
		{
			variationTween->setup( &variation, 0, 1, ofGetElapsedTimef(), animationPresetTime, TWEEN_LINEAR, TWEEN_IN, "variation" );
			variationTween->bKeepAround = true;
			
			presetMixKey = tween.addTween( presetMix, 0, 1, ofGetElapsedTimef(), animationPresetTime*.99, "presetMix" );
		}
	}
	
	//this tween handles the animtion preset blending this allows us to spend time on a preset wothout any blending
	if( bPlayAnimation && e.name == "presetMix")
	{
		if(p0 == NULL || p1 == NULL){
			p0 = &presets[ animationPresets[ animationPresetIndex0 ] ];
			p1 = &presets[ animationPresets[ animationPresetIndex1 ] ];
		}
		
		//mix current and target preset values and set the UI widget values which in turn control our variables
		if(e.message == "updated")
		{
			if(bPlayAnimation && p0 != NULL && p1 != NULL)
			{
				ofxUIWidget *w;
				for (map<string, float>::iterator it=p0->begin(); it!=p0->end(); it++)
				{
					for (int i=0; i<guis.size(); i++)
					{
						w = guis[i]->getWidget( it->first );
						if(w != NULL )
						{
							//set the slider value as a mix of p0 & p1 values assuming presetMix is between 0&1
							((ofxUISlider *)w)->setValue( ofMap(presetMix, 0, 1, (*p0)[it->first], (*p1)[it->first]) );
						}
					}
				}
			}
		}
		
		if(e.message == "ended")
		{
			animationPresetIndex0++;
			animationPresetIndex1++;
			
			if( animationPresetIndex0 >= animationPresets.size() ){
				animationPresetIndex0 = 0;
			}
			if( animationPresetIndex1 >= animationPresets.size() ){
				animationPresetIndex1 = 0;
			}
			
			p0 = &presets[ animationPresets[ animationPresetIndex0 ] ];
			p1 = &presets[ animationPresets[ animationPresetIndex1 ] ];
		}
	}
}

//--------------------------------------------------------------
void ofApp::loadShaders()
{
	cout<<endl<<endl<< "loading shaders: " << ofGetFrameNum() <<endl<<endl;
	
	//load data shader
//	facingRatioShader.load( "shaders/facingRatio" );
//	shaderMap["facingRatioShader"] = &facingRatioShader;
//	
//	normalShader.load( "shaders/normal" );
//	shaderMap["normalShader"] = &normalShader;
	
	displacedShader.load( "shaders/displaced" );
	shaderMap["displacedShader"] = &displacedShader;
	
	displacedBarsShader.load( "shaders/displacedBars" );
	shaderMap["displacedBarsShader"] = &displacedBarsShader;
	
	displacedDotsShader.load( "shaders/displacedDots" );
	shaderMap["displacedDotsShader"] = &displacedDotsShader;
	
	displacedShader.load( "shaders/displaced" );
	shaderMap["displacedShader"] = &displacedShader;
	
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
	//preset load/save
	if(bSavePreset){
		savePreset();
	}
	if(currentPresetName != nextPreset){
		if(!bCachedPresetValues){
			bCachedPresetValues = true;
			cachePresetValues();
		}
		loadPreset( nextPreset );
	}
	
	//add new journeys
	if(bJourniesNeedUpdate){
		bJourniesNeedUpdate = false;
		
		onions.resize( journeys.size() );
		
		for(int i=0; i<onions.size(); i++)
		{
			if(!onions[i].bIsSetup){
				onions[i].setup( journeys[i] );
				onions[i].color = getRandomColor();
				
				//prevent neighbor color matches
				if(i>0){
					while (onions[i].color == onions[i-1].color) {
						onions[i].color = getRandomColor();
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
}

void ofApp::retryColors(){
	for(int i=0; i<onions.size(); i++){
		onions[i].color = getRandomColor();
	}
}

//--------------------------------------------------------------
void ofApp::draw()
{
	ofPushStyle();
	
	if( captrure )	fbo.begin();
	ofClear(0,0,0,0);
		
	if (bDepthTest) {
		glEnable( GL_DEPTH_TEST);
	}else{
		glDisable(GL_DEPTH_TEST);
	}
//
	if( currentRenderType == "onion"){
		drawOnion();
	}
	
	if (!bDepthTest) {
		glDisable(GL_DEPTH_TEST);
	}
	
	if( captrure )	fbo.end();
	
	ofPopStyle();
	if( captrure ){
		captrure = false;
		
		fbo.readToPixels(outImage);
		outImage.update();
		outImage.saveImage("d_4Large"+ofGetTimestampString()+".png");
	}
//	outImage.draw( 0, 0, ofGetWidth(), ofGetHeight() );
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

void ofApp::setupOnion(){
	bOnionSetup = true;
	setupSphere();
}

void ofApp::drawOnion(){
	if(!bOnionSetup){
		setupOnion();
	}
	
	//TODO: magic number
	//update onion transforms
	ofVec3f Eul( 0, pow(sin(elapsedTime * .8), 3.)*3, pow(sin(elapsedTime * .4), 3.)*10. );
	ofQuaternion q;
	q.makeRotate(Eul.x, ofVec3f(1,0,0), Eul.y, ofVec3f(0,1,0), Eul.z, ofVec3f(0,0,1));
	
	float startScale = 1. / recursiveScale;
	
	for (int i=onions.size()-1; i>=0; i--)
	{
		if(i == onions.size()-1)
		{
			onions[i].transform.setScale( newRibbonScale,newRibbonScale,newRibbonScale * squish ); // newRibbonScale scales down to 1.
			onions[i].transform.setOrientation( q + q.inverse() * (1.f - newRibbonShaderScale) ); // newRibbonShaderScale == val btwn 0-1
		}
		else
		{
			onions[i].transform.setScale( onions[i+1].transform.getScale() * recursiveScale );
			onions[i].transform.setOrientation( onions[i+1].transform.getOrientationQuat() * q );
		}
	}
	
	globalTransform.makeIdentityMatrix();
	globalTransform.rotateRad(globalRotationAboutXAxis, 1, 0, 0);
	
	//draw it
	camera.begin();
	
	currentShader->begin();
	currentShader->setUniform1f("time", elapsedTime );
	currentShader->setUniform1f("readingThreshold", readingThreshold);
	currentShader->setUniform1f("readingScale", readingScale);
	currentShader->setUniform1f("alpha", onionAlpha);
	currentShader->setUniform1f("dataSmoothing", dataSmoothing);
	currentShader->setUniform1f("facingRatio", facingRatio);
	currentShader->setUniform1f("displacement", displacement );
	currentShader->setUniform1f("noiseScale", noiseScale );
	currentShader->setUniform1f("slope", slope );
	
	ofPushMatrix();
	ofScale( radius, radius, radius );
	
	ofPushMatrix();
	if( bRotateOnNewJourney )	ofMultMatrix( globalTransform );//<-- this rotates the onion on transition in
	
	ofRotate( newRibbonShaderScale * -360 + 90 - slope*90., 0, 0, 1);
	float vortexRotVal = -3. * elapsedTime;
	
	glEnable(GL_CULL_FACE);
	
	for (int i=0; i<onions.size(); i++) {
		
		//crazr transforms
		ofPushMatrix();
		
		ofMultMatrix( onions[i].transform.getGlobalTransformMatrix() );
		
		ofRotate( ( max( (float) onions.size() - i - 2 + newRibbonShaderScale, 0.f) ) * vortexRotVal, 0, 0, 1);
		
		//set ribbon color
		ofSetColor( onions[i].color );
		
		//per ribbon uniforms
		currentShader->setUniformTexture("dataTexture", onions[i].dataTexture, 0);
		currentShader->setUniform2f("texDim", onions[i].dataTexture.getWidth(), onions[i].dataTexture.getHeight() );
		
		if( bAddingRibbon && i == onions.size()-1 ){
			currentShader->setUniform1f("animateIn", newRibbonShaderScale );
		}
		else{
			currentShader->setUniform1f("animateIn", 1 );
		}
		
		//draw front and back in diferent passes
		glCullFace(GL_BACK);
		sphereVbo.drawElements( GL_TRIANGLES, spherVboIndexCount );
		
		glCullFace(GL_FRONT);
		sphereVbo.drawElements( GL_TRIANGLES, spherVboIndexCount );
		
		
		ofPopMatrix();
		
	}
	
	glDisable(GL_CULL_FACE);
	
	ofPopMatrix();
	
	ofPopMatrix();
	
	
	currentShader->end();
	
	camera.end();
	
	if(guis[0]->isVisible()){
		ofSetColor(255,255,255);
		for (int i=0; i<onions.size(); i++) {
			onions[i].dataTexture.draw(5, ofGetHeight() - 10 - 5*i, ofGetWidth()/4, 4.5 );
		}
	}
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
	
	if(bDebug)	cout << "saving preset working" << endl;
	savePreset("Working");
	
	
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
	
	if(bDebug)	cout << "end of ofApp::exit" << endl;
	
	//???: we're getting this error every now and again on exit:
	//	libc++abi.dylib: terminate called throwing an exception
}


//--------------------------------------------------------------
ofColor ofApp::getRandomColor(){
	
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

void ofApp::cachePresetValues(){
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
					
				if( kind == 4 ){
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
		cout << "Presets/" + presetName + "/"+guis[i]->getName()+".xml" << endl;
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
	if(key == 's' || key == 'S' ) {
		savePreset();
	}
	
	if(key == ' '){
		loadShaders();
	}
	
	if(key == 'c'){
		retryColors();
	}
	
	if(key == 'e' || key == 'E'){
//		captrure = true;
	}
	
	if(key == ' '){
		if(!bAddingRibbon){
			addRibbonTween = tween.addTween( addRibbonVal, 0, 1, ofGetElapsedTimef(), 4, "addRibbon", TWEEN_SMOOTHERSTEP );
			
			//load a journey from file
			ofBuffer buffer = ofBufferFromFile("Journeys/journey_showJourney" + ofToString(int(ofRandom(4))) + ".json");
			if(buffer.size()){
				reader.parse( buffer.getText(), json );
				handleRoute( json );
			}
		}
	}
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
}

//--------------------------------------------------------------
void ofApp::onClose( ofxLibwebsockets::Event& args )
{
    cout<<"on close"<<endl;
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
		
		cout << ( journeyDirectory + "journey_" + route + fileIndex  +".json").c_str() << endl;
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
	
    else if(route=="addJourney") {
			
		cout << "addJourney" << endl;
		//true for animating in
        journeys.push_back( new Journey(json["journey"], true) );
		bJourniesNeedUpdate = true;
		
		addRibbonTween = tween.addTween( addRibbonVal, 0, 1, ofGetElapsedTimef(), 4, "addRibbon", TWEEN_SINUSOIDAL, TWEEN_INOUT );
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


