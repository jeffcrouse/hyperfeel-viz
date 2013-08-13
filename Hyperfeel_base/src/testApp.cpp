#include "testApp.h"

//???: do we need to be able to skew the projection?


//--------------------------------------------------------------
void testApp::setup(){
	ofSetVerticalSync(true);
		
	//fbo
	fbo.allocate( ofGetWidth(), ofGetHeight(), GL_RGBA, 4 );
	
	//shaders
	loadShaders();
	
	//data
	attention.resize( 1000 * 2 );
	meditation.resize( 1000 * 2 );
	timeStamp.resize( 1000 * 2 );
	plusMinus.resize( 1000 * 2 );
	
	float numSamples = 1000;
	for (int i=0; i<numSamples; i++) {
		attention[i] = ofNoise( float(i * 100) / float(attention.size()) );
		meditation[i] = 1. - attention[i];
		timeStamp[i] = float(i) / numSamples;
		plusMinus[i] = 1;
	}
	for (int i=numSamples; i<numSamples * 2; i++) {
		attention[i] = ofNoise( float(i * 100) / float(attention.size()) );
		meditation[i] = 1. - attention[i];
		timeStamp[i] = float(i-numSamples) / numSamples;
		plusMinus[i] = -1;
	}
	vector<ofVec3f> vertices( attention.size() );
		
	float step = 1. / numSamples;
	for (int i=0; i<attention.size(); i++) {
		vertices[i].set( step * float(i) * 2. - 1., 0, 0 );
	}
	
	vbo.setVertexData( &vertices[0], vertices.size(), GL_STATIC_DRAW );
	
	
	dataShader.begin();
	vbo.setAttributeData( dataShader.getAttributeLocation( "attention" ), &attention[0], 1, attention.size(), GL_STATIC_DRAW );
	vbo.setAttributeData( dataShader.getAttributeLocation( "meditation" ), &meditation[0], 1, meditation.size(), GL_STATIC_DRAW );
	vbo.setAttributeData( dataShader.getAttributeLocation( "timeStamp" ), &timeStamp[0], 1, timeStamp.size(), GL_STATIC_DRAW );
	vbo.setAttributeData( dataShader.getAttributeLocation( "plusMinus" ), &plusMinus[0], 1, plusMinus.size(), GL_STATIC_DRAW );
	dataShader.end();
	
	
	//GUI
	bHide = true;
	
	gui.setup("panel"); // most of the time you don't need a name but don't forget to call setup
	gui.add(radius.set( "radius", 512, 256, 1024 ));
	gui.add(color.set("color",ofColor(100,100,140),ofColor(0,0),ofColor(255,255)));
	
	gui.add( bckgrnd0.set("bckgrnd0",ofColor(100,100,140),ofColor(0,0),ofColor(255,255)));
	gui.add( bckgrnd1.set("bckgrnd1",ofColor(100,100,140),ofColor(0,0),ofColor(255,255)));
	
	gui.add( curveOffset.set( "curveOffset",100, -400, 400) );
	gui.add( curveRadius.set( "curveRadius",100, 1, 1000) );
	
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
	
	//	camera.begin();
	ofPushMatrix();
	ofTranslate( ofGetWidth()/2, ofGetHeight()/2);
	dataShader.begin();
	dataShader.setUniform1f("radius", curveRadius );
	dataShader.setUniform1f("offset", curveOffset );
	vbo.draw(GL_LINE_STRIP, 0, attention.size() );
	dataShader.end();
	ofPopMatrix();
	//	camera.end();
	
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
