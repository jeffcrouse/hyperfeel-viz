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
	
	bOnionSetup = false;
	
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
	guiMain->addLabel("render Types");
	guiMain->addRadio("renderTypes", renderTypes );
	
	guiMain->addLabel("main");
	guiMain->addSpacer();
	guiMain->addFPS();
	guiMain->addSpacer();
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
	
	//create a radio for switching renderTypes
	guiMain->addSpacer();
	guiMain->addLabel("Shaders" );
	cout << "shaderNames.size(): "<< shaderNames.size() << endl;
	guiMain->addRadio("shaders", shaderNames );
	
	
//	vector<string> glDrawTypes;
//	glDrawTypes.push_back("GL_QUADS");
//	glDrawTypes.push_back("GL_LINES");
//	ribbonDrawType = GL_QUADS;
//	guiMain->addSpacer();
//	
//	guiMain->addRadio("ribbonDrawType", glDrawTypes );
	
	
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
}

void ofApp::guiEvent(ofxUIEventArgs &e){
	
	string name = e.widget->getName();
	int kind = e.widget->getKind();
	//	cout << endl << name << " : " << kind << " : " << e.widget->getParent()->getName() <<  endl;
	
	
	if( name == "save preset" ){
		bSavePreset = true;
		//savePreset();
	}
	
//	else if( name == "GL_QUADS"){
//		if(e.getToggle()->getValue()){
//			ribbonDrawType = GL_QUADS;
//		}
//	}
//	
//	else if( name == "GL_LINES"){
//		if(e.getToggle()->getValue()){
//			ribbonDrawType = GL_LINES;
//		}
//	}
	
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

//--------------------------------------------------------------
void ofApp::loadShaders()
{
	cout<<endl<<endl<< "loading shaders: " << ofGetFrameNum() <<endl<<endl;
	
	//load data shader
	facingRatioShader.load( "shaders/facingRatio" );
	shaderMap["facingRatioShader"] = &facingRatioShader;
	
	normalShader.load( "shaders/normal" );
	shaderMap["normalShader"] = &normalShader;
	
	displacedShader.load( "shaders/displaced" );
	shaderMap["displacedShader"] = &displacedShader;
	
	displacedBarsShader.load( "shaders/displacedBars" );
	shaderMap["displacedBarsShader"] = &displacedBarsShader;
	
	displacedDotsShader.load( "shaders/displacedDots" );
	shaderMap["displacedDotsShader"] = &displacedDotsShader;
	
	displacedShader.load( "shaders/displaced" );
	shaderMap["displacedShader"] = &displacedShader;
	
	onionShader.load( "shaders/onion" );
	shaderMap["onionShader"] = &onionShader;
	
	onionBarsShader.load( "shaders/onionBars" );
	shaderMap["onionBarsShader"] = &onionBarsShader;
	
	onionBarsHorzShader.load( "shaders/onionBarsHorz" );
	shaderMap["onionBarsHorzShader"] = &onionBarsHorzShader;
	
	onionDotsShader.load( "shaders/onionDots" );
	shaderMap["onionDotsShader"] = &onionDotsShader;
	
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
		loadPreset( nextPreset );
	}
	
	//add new journeys
	if(bJourniesNeedUpdate){
		bJourniesNeedUpdate = false;
		
		onions.resize( journeys.size() );
		
		for(int i=0; i<onions.size(); i++){
			onions[i].setup( journeys[i] );
			onions[i].color = getRandomColor();
		}
	}
	
	//animation
	elapsedTime = ofGetElapsedTimef();
	float t = elapsedTime * 3;
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
	ofBackground( 0,0,0 );
	
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
	
	
	ofPopStyle();
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
	
	camera.begin();
	
	currentShader->begin();
	currentShader->setUniform1f("time", elapsedTime );
	
//	sphereVbo.drawElements(GL_QUADS, spherVboIndexCount );
	
	ofVec3f Eul( 0, pow(sin(elapsedTime * .8), 3.)*3, pow(sin(elapsedTime * .4), 3.)*10. );
	for (int i=0; i<onions.size(); i++) {
		
		if(i>0){
			onions[i].transform.setScale( onions[i-1].transform.getScale() * recursiveScale );
			onions[i].transform.setOrientation( onions[i-1].transform.getOrientationEuler() + Eul );
		}else{
			onions[i].transform.setScale( 1 );
			onions[i].transform.setOrientation( Eul );
		}
	}
	
	float scaleStep = 1./float(onions.size());
	ofPushMatrix();
	ofScale( radius, radius, radius * squish );
	
	glEnable(GL_CULL_FACE);
	for (int i=onions.size()-1; i>=0; i--) {
		
		ofPushMatrix();
		ofMultMatrix( onions[i].transform.getGlobalTransformMatrix() );
		ofRotate((i*elapsedTime)*3., 0, 0, 1);
		
		ofSetColor( onions[i].color );
		
//		if(currentShader == &displacedShader){
		currentShader->setUniform1f("displacement", displacement );
//		}

		currentShader->setUniformTexture("dataTexture", onions[i].dataTexture, 0);
		currentShader->setUniform2f("texDim", onions[i].dataTexture.getWidth(), onions[i].dataTexture.getHeight() );
		currentShader->setUniform1f("readingThreshold", readingThreshold);
		currentShader->setUniform1f("readingScale", readingScale);
		currentShader->setUniform1f("alpha", onionAlpha);
		currentShader->setUniform1f("dataSmoothing", dataSmoothing);
		currentShader->setUniform1f("facingRatio", facingRatio);
		
		glCullFace(GL_BACK);
		sphereVbo.drawElements( GL_TRIANGLES, spherVboIndexCount );
		
		glCullFace(GL_FRONT);
		sphereVbo.drawElements( GL_TRIANGLES, spherVboIndexCount );
		
		
		ofPopMatrix();
		
	}
	
	glDisable(GL_CULL_FACE);
	
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
			
			if(_json["journeys"][i]["readings"].size() > 100){
				//false for not animating in
				journeys.push_back(new Journey(_json["journeys"][i], false));
			}
			
        }
		
		if(journeys.size() < 20){
			for(int i=0; i<_json["journeys"].size(); i++) {
				
				if(_json["journeys"][i]["readings"].size() > 100){
					//false for not animating in
					journeys.push_back(new Journey(_json["journeys"][i], false));
				}
			}
		}
		
		
		if(journeys.size() < 40){
			for(int i=0; i<_json["journeys"].size(); i++) {
				
				if(_json["journeys"][i]["readings"].size() > 100){
					//false for not animating in
					journeys.push_back(new Journey(_json["journeys"][i], false));
				}
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
