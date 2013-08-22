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
    options.host = "laserstorms-MacBook-Pro.local"; //
    options.port = 8080;
    bool connected = client.connect( options );
    
    client.addListener(this);
    ofSetFrameRate(60);
    
//    ofSetLogLevel(OF_LOG_VERBOSE);
	
	//Journy stuff
	bSaveJsonsToFile = false;//for debuggin it was faster to load them from file rather then wait for the server
	bLoadJsonsFromFile = true;
	bJourniesNeedUpdate = false;
	
	//	journey colors
	colorMap["red"].set( 255, 99, 99 );				// = ofColor::red;
	colorMap["orange"].set( 255, 182, 42 );			// = ofColor::orange;
	colorMap["yellow"].set( 236,  236, 146 );		// = ofColor::yellow;
	colorMap["green"].set( 79, 230, 60 );			// = ofColor::green;
	colorMap["blue"].set( 4, 184, 197 );			// = ofColor::blue;
	colorMap["indigo"].set( 131, 102, 212 );		// = ofColor::indigo;
	colorMap["violet"].set( 227, 59, 207 );			// = ofColor::violet;
		
	fbo.createAndAttachTexture(GL_RGBA, 1);
	fboMap.allocate( 1024, 1024, GL_RGBA, 4 );
	
	// -
	loadShaders();
	
	// ---
	setDefaults();
	
    // ---
	setupUI();
	
	currentPresetName = "default";
	
	if(bLoadJsonsFromFile){
		cout << "loading journeys from files" << endl;
		ofBuffer buffer = ofBufferFromFile("Journeys/journey_init.json");
		reader.parse( buffer.getText(), json );
		handleRoute( json );
	}

}


void ofApp::setDefaults(){
	camera.setFarClip(20000);
	camera.setNearClip(20);
	
	radius = 10;
	nearDepthScale = 3000;
	farDepthScale = 3500;
	
	bDepthTest = true;
}


void ofApp::setupUI(){
	
	renderTypes.clear();
	renderTypes.push_back("rainbowLayers");
	renderTypes.push_back("displacedMesh");
	renderTypes.push_back("russianDolls");
	
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
	
	guiMain->addLabel("main");
	guiMain->addSpacer();
	guiMain->addFPS();
	guiMain->addSpacer();
	guiMain->addLabel("global rendering");
	guiMain->addToggle("depthTest", bDepthTest);
	

	guiMain->addSlider("minRibbonScale", 0.1, 50.0, &minRibbonScale );
	guiMain->addSlider("maxRibbonScale", 1., 100.0, &maxRibbonScale );
	guiMain->addSlider("twistAngle", 0., 20.0, &twistAngle );
	guiMain->addSlider("frExpo", .6, 20, &frExpo );
	guiMain->addSlider("radius", 1, 20, &radius );
	guiMain->addSlider("nearDepthScale", 2000, 4000, &nearDepthScale );
	guiMain->addSlider("farDepthScale", 3000, 10000, &farDepthScale );
//	guiMain->addSlider("goboRadius", 256, 1024, &goboRadius );
	
	//create a radio for switching renderTypes
	guiMain->addSpacer();
	guiMain->addLabel("Shaders" );
//	guiMain->addRadio("render types", renderTypes );
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
	presetGui->setPosition( guiMain->getRect()->getX(), guiMain->getRect()->getY() + guiMain->getRect()->getHeight() + 30);
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
}

void ofApp::guiEvent(ofxUIEventArgs &e){
	
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
	}
	
}

//--------------------------------------------------------------
void ofApp::loadShaders()
{
	cout<<endl<<endl<< "loading shaders: " << ofGetFrameNum() <<endl<<endl;
	
	//load data shader
	journeyLineShader.load( "shaders/dataLineShader" );
	currentShader = &journeyLineShader;
	
	ribbonSine.load("shaders/dataLineShader.vert", "shaders/ribbonSine.frag");
	
	ribbonDepth.load("shaders/dataLineShader.vert", "shaders/ribbonDepth.frag");
	
	ribbonTransparent.load( "shaders/dataLineShader.vert", "shaders/ribbonTransparent.frag");
	
	ribbonTransparentNoise.load( "shaders/dataLineShader.vert", "shaders/ribbonTransparentNoise.frag");
	
	ribbonBars.load( "shaders/dataLineShader.vert", "shaders/ribbonBars.frag");
	
	ribbonRings.load( "shaders/dataLineShader.vert", "shaders/ribbonRings.frag");
	
	
	shaderMap["dataLineShader"] = &journeyLineShader;
	shaderMap["ribbonSine"] = &ribbonSine;
	shaderMap["ribbonDepth"] = &ribbonDepth;
	shaderMap["ribbonTransparent"] = &ribbonTransparent;
	shaderMap["ribbonTransparentNoise"] = &ribbonTransparentNoise;
	shaderMap["ribbonBars"] = &ribbonBars;
	shaderMap["ribbonRings"] = &ribbonRings;
	
	//store the shader names in a vecotr for use in a radio gui
	for( map<string, ofShader*>::iterator it = shaderMap.begin(); it!= shaderMap.end(); it++){
		shaderNames.push_back(it->first);
	}
}


//--------------------------------------------------------------
void ofApp::update()
{
	//preset load/save
	if(bSavePreset){
		savePreset();
	}
	if(currentPresetName != nextPreset){
		loadPreset( nextPreset );
	}
	
	//add new journeys
	if(bJourniesNeedUpdate){
		bJourniesNeedUpdate = false;
		
		if(bDebug)	cout << "update journeys " << ofGetElapsedTimef() << endl;
		if(bDebug)	cout << "journeys.size(): " << journeys.size() << endl;
		
		
		ribbons.resize( journeys.size() );
		for (int i=0; i<ribbons.size(); i++) {
			ribbons[i].setup( journeys[i], getRandomColor() );
		}
	}
	
	//animation
	elapsedTime = ofGetElapsedTimef();
	float t = elapsedTime * 3;
	float loadDataVal = t - floor(t);
}

void ofApp::retryColors(){
	for(int i=0; i<ribbons.size(); i++){
		ribbons[i].color = getRandomColor();
	}
}

//--------------------------------------------------------------
void ofApp::draw()
{
	ofPushStyle();
	ofBackground( 0,0,0 );
	
	if (bDepthTest) {
		glEnable( GL_DEPTH_TEST);
	}else{
		glDisable(GL_DEPTH_TEST);
	}
//
//	if( currentPresetName == "default"){
//		//draw our stuff
	
	ofEnableAlphaBlending();
	ofBlendMode( OF_BLENDMODE_ADD );
	glLineWidth( 3 );
	
	ofSetColor(255, 0, 0);
	
	glEnable(GL_DEPTH_TEST);
	camera.begin();
	ofPushMatrix();
	ofScale(20, 20, 20);
	
	float scl;
	
	currentShader->begin();
	currentShader->setUniform1f("frExpo", frExpo);
	currentShader->setUniform1f("radius", radius);
	currentShader->setUniform1f("nearDepthScale", nearDepthScale);
	currentShader->setUniform1f("farDepthScale", farDepthScale);
	for (int i=ribbons.size()-1; i>=0; i--) {
		
		ofSetColor( ribbons[i].color );
		
		ofPushMatrix();
		ofTranslate(0, 0, -100 + -10 * i );
		ofRotate( (i+1.) * elapsedTime * .6, 0, 0, 1 );
		scl = ofMap(i, 0, ribbons.size()-1, maxRibbonScale, minRibbonScale);
		ofScale( scl , scl);
		
		currentShader->setUniform1f("time", elapsedTime + float(i) * .2 );
		currentShader->setUniform1f("spread", ofMap(float(i), 0., ribbons.size()-1., 0, 1) );
//		currentShader->setUniform1f("twistAngle", 0. );// ofMap(float(i), 0., ribbons.size()-1., .0, 1.));
		currentShader->setUniform1i("twist", 1 );
		currentShader->setUniform1f("twistAngle", twistAngle );
		
		ribbons[i].draw();
		
		
		ofPopMatrix();
		
	}
	currentShader->end();
	
	
	glDisable(GL_DEPTH_TEST);
	ofPopMatrix();
	camera.end();
		
//	}
	
	if (!bDepthTest) {
		glDisable(GL_DEPTH_TEST);
	}
	
	
	ofPopStyle();
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
void ofApp::windowResized(int w, int h){
//    screenSize = ofToString(w) + "x" + ofToString(h);
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
	
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
	
}





//--------------------------------------------------------------
void ofApp::onConnect( ofxLibwebsockets::Event& args ){
    cout<<"on connected"<<endl;
}

//--------------------------------------------------------------
void ofApp::onOpen( ofxLibwebsockets::Event& args ){
    cout<<"on open"<<endl;
}

//--------------------------------------------------------------
void ofApp::onClose( ofxLibwebsockets::Event& args ){
    cout<<"on close"<<endl;
}

//--------------------------------------------------------------
void ofApp::onIdle( ofxLibwebsockets::Event& args ){
    cout<<"on idle"<<endl;
}

//--------------------------------------------------------------
void ofApp::handleRoute(Json::Value& _json){
	string route = _json["route"].asString();
	
	if(bSaveJsonsToFile){
		
		ofDirectory dir;
		string journeyDirectory = ofToDataPath("Journeys/");
		if(!dir.doesDirectoryExist(journeyDirectory))	dir.createDirectory(journeyDirectory);
		
		cout << ( journeyDirectory + "journey_" + route +".json").c_str() << endl;
		ofstream    fs( ( journeyDirectory + "journey_" + route +".json").c_str() );
		
		fs << _json << endl;
		
		fs.close();
	}
    
    if(route=="init") {
		if(bDebug)	cout << "message == init" << endl;
		// on start up to populate the animation
		vector <string> outStrings;
        for(int i=0; i<_json["journeys"].size(); i++) {
			
			if(_json["journeys"][i]["readings"].size() > 180){
				//false for not animating in
				journeys.push_back(new Journey(_json["journeys"][i], false));
			}
			
        }
		
		bJourniesNeedUpdate = true;
    }
    else if(route=="journey") {
		if(bDebug)	cout << "message == journey" << endl;
		
		//true for animating in
        journeys.push_back(new Journey(json["journey"], true));
		
		
		bJourniesNeedUpdate = true;
		
    }
    else if(route=="tick") {
		//
		//		//for testing, can go if iritating
		//        ofLogVerbose() << "server time: " << iso8601toTimestamp(json["date"].asString());
    }
    else {
        ofLogWarning() << "Route " << route << " unknown..." << endl;
    }
	
}
void ofApp::onMessage( ofxLibwebsockets::Event& args ){
//cout<<"got message "<< args.message <<  endl;
    
    if ( !reader.parse( args.message, json ) ) {
        std::cout  << "Failed to parse json\n" << reader.getFormattedErrorMessages();
        return;
    }
    
	if(!bLoadJsonsFromFile)	handleRoute(json);
	
}

//--------------------------------------------------------------
void ofApp::onBroadcast( ofxLibwebsockets::Event& args ){
    cout<<"got broadcast "<<args.message<<endl;
}
