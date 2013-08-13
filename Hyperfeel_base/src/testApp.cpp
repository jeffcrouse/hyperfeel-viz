#include "testApp.h"

//???: do we need to be able to skew the projection?


//--------------------------------------------------------------
void testApp::setup(){
	ofSetVerticalSync(true);
		
	//fbo
	fbo.allocate( ofGetWidth(), ofGetHeight(), GL_RGBA, 4 );
	
	//shaders
	loadShaders();
	
	//make some fake data
	
	vector <float> attention, meditation, timeStamp;
	attention.resize( 1000 );
	meditation.resize( 1000 );
	timeStamp.resize( 1000 );
	float numSamples = 1000;
	for (int i=0; i<numSamples; i++) {
		attention[i] = ofNoise( float(i * 40) / (numSamples-1.) );
		meditation[i] = ofNoise( float(i * 40) / (numSamples-1.) + 77);
		timeStamp[i] = float(i) / numSamples;
	}
	
	//camera
	camera.setNearClip(1);
	camera.setFarClip(500);
	camera.setFov( 60 );
	
	//create vertex data
	numVertices = attention.size() * 2;
	vector<ofVec3f> vertices( numVertices );
	
	for (int i=0; i<numVertices; i+=2	) {
		vertices[i].set( attention[i/2], timeStamp[i/2], -1 );
		vertices[i+1].set( meditation[i/2], timeStamp[i/2], 1 );
	}
		
	//close the ribbon
	vertices.push_back( ofVec3f( attention[0], timeStamp[0], -1 ) );
	vertices.push_back( ofVec3f( meditation[0], timeStamp[0], 1 ) );
	numVertices += 2;
	
	//set the vbo.. we need to use vertices or else ofVbo won't draw...
	vbo.setVertexData( &vertices[0], vertices.size(), GL_STATIC_DRAW );

//	dataShader.begin();
//	vbo.setAttributeData( dataShader.getAttributeLocation( "vData" ), &vData[0], 1, vData.size(), GL_STATIC_DRAW );
//	vbo.setAttributeData( dataShader.getAttributeLocation( "timeStamp" ), &vTimeStamp[0], 1, vTimeStamp.size(), GL_STATIC_DRAW );
//	vbo.setAttributeData( dataShader.getAttributeLocation( "plusMinus" ), &vPlusMinus[0], 1, vPlusMinus.size(), GL_STATIC_DRAW );
//	dataShader.end();
	
	
	//GUI
	bHide = true;
	
	gui.setup("panel"); // most of the time you don't need a name but don't forget to call setup
	gui.add( fpsLabel.set("fps", "60") );
	gui.add(radius.set( "radius", 512, 256, 1024 ));
//	gui.add(color.set("color",ofColor(100,100,140),ofColor(0,0),ofColor(255,255)));
	
	gui.add( cameraGroup.setup( "camera" ) );
	cameraGroup.add( nearClip.set("nearClip", 10, 1, 500) );
	cameraGroup.add( farClip.set("farClip", 500, 500, 5000) );
	cameraGroup.add( fov.set("fov", 60, 1, 90) );
	
	gui.add( bckgrnd0.set("bckgrnd0",ofColor(100,100,140),ofColor(0,0),ofColor(255,255)));
	gui.add( bckgrnd1.set("bckgrnd1",ofColor(100,100,140),ofColor(0,0),ofColor(255,255)));
	
	gui.add( curveOffset.set( "curveOffset",100, -400, 400) );
	gui.add( curveRadius.set( "curveRadius",100, 1, 1000) );
	gui.add( curveWidth.set( "curveWidth",10, 1, 100) );
	
	gui.add( presetGroup.setup( "presets" ) );
	presetGroup.add( savePresetButton.setup("save preset"));
	savePresetButton.addListener( this, &testApp::savePreset );
	
	
	loadPresetsToGui();
}


//--------------------------------------------------------------
void testApp::loadShaders(){
	//try hitting "l" to reload
	
	cout<<endl<<endl<< "loading shaders: " << ofGetFrameNum() <<endl<<endl;
	fboShader.load("shaders/fboShader");
	
	dataShader.load("shaders/dataShader");
	
}


//--------------------------------------------------------------
void testApp::update(){
	//	ofSetCircleResolution(circleResolution);
	fpsLabel.set( ofToString( ofGetFrameRate()));
}

//--------------------------------------------------------------
void testApp::draw(){
	//
	ofBackground( 0,0,0 );
	
	//Draw out scene to the fbo
	fbo.begin();
	ofClear(0,0,0);
    ofBackgroundGradient( bckgrnd0, bckgrnd1 );
	
	glLineWidth( 3 );
	
	camera.setFov( fov );
	camera.setNearClip( nearClip );
	camera.setFarClip( farClip );
	
	camera.setFov( fov );
	camera.begin();
	dataShader.begin();
	dataShader.setUniform1f("nearClip", nearClip );
	dataShader.setUniform1f("farClip", farClip );
	dataShader.setUniform1f("curveWidth", curveWidth );
	for (int i=0; i < 30; i++) {
		
		ofColor c;
		c.setHsb(255 * float(i)/30, 200, 255);
		
		dataShader.setUniform3f("color", float(c.r)/255., float(c.g)/255., float(c.b)/255. );
		dataShader.setUniform1f("radius", curveRadius * (1. - (float(i)/30)*.98) );
		dataShader.setUniform1f("offset", curveOffset * (1. - (float(i)/30)*.98) );
		
		ofPushMatrix();
//		ofTranslate( ofGetWidth()/2, ofGetHeight()/2, 0);
		ofTranslate( 0, 0, -30 * i);
		ofRotate( i * 20., 0, 0, 1);
		
		vbo.draw(GL_QUAD_STRIP, 0, numVertices );
		
		ofPopMatrix();
		
	}
	dataShader.end();
	camera.end();
	
	fbo.end();
	
	
	//draw fbo to the screen
	ofSetColor(255, 255, 255 );
	fboShader.begin();
	fboShader.setUniform1f( "radius", radius );
	fboShader.setUniform2f( "texDim", fbo.getWidth(), fbo.getHeight() );
	fboShader.setUniformTexture("tex", fbo.getTextureReference(), 0 );
	fbo.draw(0, 0, ofGetWidth(), ofGetHeight() );
	fboShader.end();
	
	
	glDisable(GL_DEPTH_TEST);


	
	glEnable(GL_DEPTH_TEST);
	
	
	//GUI
	if( bHide ){
		glDisable(GL_DEPTH_TEST);
		gui.draw();
		glEnable(GL_DEPTH_TEST);
	}
}


//--------------------------------------------------------------
void testApp::exit(){
	
	gui.saveToFile("presets/working.xml");

	//remove preset listeners
	for (map<string, Hyperfeel_Button >::iterator it=presetNames.begin(); it!=presetNames.end(); ++it){
		it->second.removeListener( this, &testApp::switchPresets );
	}
	
	
	savePresetButton.removeListener( this, &testApp::savePreset );
}



//--------------------------------------------------------------
void testApp::loadPresetsToGui(){
	
	//some path, may be absolute or relative to bin/data
	string path = "presets/";
	ofDirectory dir(path);
	dir.allowExt("xml");
	dir.listDir();
	
	//go through and store our preset names
	for(int i = 0; i < dir.numFiles(); i++){
		//get a clean name
		string cleanName = dir.getName(i);
		cleanName.erase(cleanName.end()-4,cleanName.end());
		
		presetNames[ cleanName ];//.setup( cleanName, false );
		
	}
	
	//add our presets to the gui
	for (map<string, Hyperfeel_Button >::iterator it=presetNames.begin(); it!=presetNames.end(); ++it){
		presetGroup.add( it->second.setup(it->first, false) );
		it->second.addListener( this, &testApp::switchPresets );
	}
	
	//load our working settings. always working from working...
	gui.loadFromFile( "presets/working.xml" );
	currentPresetName = "working";
}

void testApp::addPresetToGui( string name ){
	presetGroup.add( presetNames[ name ].setup( name, false) );
	presetNames[ name ].addListener( this, &testApp::switchPresets );
}

void testApp::updateGuiPresets(){
	//some path, may be absolute or relative to bin/data
	string path = "presets/";
	ofDirectory dir(path);
	dir.allowExt("xml");
	dir.listDir();
	
	for(int i = 0; i < dir.numFiles(); i++){
		string cleanName = dir.getName(i);
		cleanName.erase(cleanName.end()-4,cleanName.end());
		
		//if it isn't added to the gui add it
		if(presetNames.find( cleanName ) == presetNames.end())	addPresetToGui( cleanName );
	}
}

void testApp::switchPresets(){
	
	string lastPreset = currentPresetName;
	for (map<string, Hyperfeel_Button >::iterator it=presetNames.begin(); it!=presetNames.end(); ++it){
		if( it->second.getValue() == true && it->first != lastPreset ){
			currentPresetName = it->first;
		}
	}
	
	presetNames[lastPreset].setValue( lastPreset == currentPresetName );
	
	loadPreset( currentPresetName );
}

void testApp::loadPreset( string name){
	gui.loadFromFile( "presets/" + currentPresetName + ".xml" );
}

void testApp::savePreset(){
	ofFileDialogResult saveFileResult = ofSystemSaveDialog( "preset", "Save your preset");
	if (saveFileResult.bSuccess){
		if( saveFileResult.fileName.size() < 4 || saveFileResult.fileName.substr( saveFileResult.fileName.size()-4, saveFileResult.fileName.size() ) != ".xml"){
			saveFileResult.fileName += ".xml";
		}
		gui.saveToFile("presets/" + saveFileResult.fileName );
		
		updateGuiPresets();
	}
}


//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if( key == 'h' ){
		bHide = !bHide;
	}
	if(key == 's' || key == 'S' ) {
		savePreset();
	}
	
	if(key == ' '){
//		reloadShaders
		loadShaders();
	}
}
//--------------------------------------------------------------
void testApp::keyReleased(int key){
	
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
//    screenSize = ofToString(w) + "x" + ofToString(h);
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){
	
}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){
	
}
