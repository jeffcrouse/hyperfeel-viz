#include "testApp.h"

//???: do we need to be able to skew the projection?


//--------------------------------------------------------------
void testApp::setup(){
	ofSetVerticalSync(true);
		
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
	
	fbo.createAndAttachTexture(GL_RGBA16, 1);
	
	cout << "fbo.getNumTextures(): " << fbo.getNumTextures() << endl;
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
//	camera.setNearClip(1);
//	camera.setFarClip(500);
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
	
	gui.add( cameraGroup.setup( "camera" ) );
	cameraGroup.add( nearClip.set("nearClip", 10, 1, 500) );
	cameraGroup.add( farClip.set("farClip", 500, 500, 5000) );
	cameraGroup.add( fov.set("fov", 60, 1, 90) );
	
	gui.add( bckgrnd0.set("bckgrnd0",ofColor(100,100,140),ofColor(0,0),ofColor(255,255)));
	gui.add( bckgrnd1.set("bckgrnd1",ofColor(100,100,140),ofColor(0,0),ofColor(255,255)));
		
	gui.add( curveOffset.set( "curveOffset",100, -400, 400) );
	gui.add( curveRadius.set( "curveRadius",100, 1, 1000) );
	gui.add( curveWidth.set( "curveWidth",10, 1, 100) );
	
	
	gui.add( minThreshold.set( "minThreshold", .01, .001, .2 ));
	gui.add( maxThreshold.set( "maxThreshold", .1, .01, .5 ));
	gui.add( sampleRadius.set( "sampleRadius", 40, 2, 1000 ));
	
	gui.add( presetGroup.setup( "presets" ) );
	presetGroup.add( savePresetButton.setup("save preset"));
	savePresetButton.addListener( this, &testApp::savePreset );
	
	
	loadPresetsToGui();
}


//--------------------------------------------------------------
void testApp::loadShaders(){
	//try hitting "l" to reload
	
	cout<<endl<<endl<< "loading shaders: " << ofGetFrameNum() <<endl<<endl;
	//some points distributed on half a sphere
    float halfSphereSamps[] = {
        -0.717643236477, -0.688175618649, 0.273249441045,
        -0.305361618869, -0.95300924778, 0.101219129631,
        0.305361649948, -0.95300924778, 0.101219129631,
        0.717643236477, -0.688175618649, 0.273249441045,
        -0.448564588874, -0.507219731808, 0.753300700721,
        -0.0284153218579, -0.664495944977, 0.753300700721,
        -0.692949481281, 0.0272486507893, 0.753300633352,
        -0.345248291778, -0.271523624659, 0.999093570452,
        0.283150254848, -0.33107188344, 0.999093570452,
        0.528938805438, -0.430145829916, 0.753300633352,
        -0.528938805438, 0.430145919323, 0.753300633352,
        -0.283150192691, 0.331071943045, 0.999093570452,
        0.345248322856, 0.271523594856, 0.999093570452,
        0.692949481281, -0.0272486060858, 0.753300633352,
        -0.717643112163, 0.688175678253, 0.273249407361,
        0.028415399554, 0.664495944977, 0.753300700721,
        0.448564588874, 0.507219791412, 0.753300633352,
        0.71764317432, 0.273249373677, 0.688175618649,
        -0.305361525634, 0.953009307384, 0.101219028578,
        0.305361649948, 0.95300924778, 0.101219062263,
        0.993816845531, 0.292822957039, 0.101219028578,
        0.993816845531, -0.292822986841, 0.101219028578,
        -0.993816845531, 0.292823016644, 0.101219028578,
        -0.993816907688, -0.29282292723, 0.101219062263
    };
	
	
	fboShader.load("shaders/fboShader");
	fboShader.begin();
    fboShader.setUniform3fv("samples", halfSphereSamps, 24*3);
	fboShader.end();
	
	dataShader.load("shaders/dataShader");
	
}


//--------------------------------------------------------------
void testApp::update(){
	elapsedTime = ofGetElapsedTimef();
	//	ofSetCircleResolution(circleResolution);
	fpsLabel.set( ofToString( ofGetFrameRate()));
}

//--------------------------------------------------------------
void testApp::draw(){
	//
	ofBackground( 0,0,0 );
	
	glEnable(GL_DEPTH_TEST);
	
	//Draw out scene to the fbo
	fbo.begin();
    fbo.activateAllDrawBuffers();
	ofClear(0,0,0);
    ofBackgroundGradient( bckgrnd0, bckgrnd1 );
	
	//set camera attributes
	camera.setFov( fov );
//	camera.setNearClip( nearClip );
//	camera.setFarClip( farClip );
	
	//draw the data
	camera.begin();
	dataShader.begin();
	dataShader.setUniform1f("nearClip", nearClip );
	dataShader.setUniform1f("farClip", farClip );
	dataShader.setUniform1f("curveWidth", curveWidth );
	ofColor c;
	for (int i=0; i < 30; i++) {
		
		c.setHsb(255 * float(i)/30, 150, 255);
		
		dataShader.setUniform3f("color", float(c.r)/255., float(c.g)/255., float(c.b)/255. );
		dataShader.setUniform1f("radius", curveRadius * (1. - (float(i)/30)*.98) );
		dataShader.setUniform1f("offset", curveOffset * (1. - (float(i)/30)*.98) );
		
		ofPushMatrix();
		ofTranslate( 0, 0, -30 * i);
		ofRotate( i * 20. + elapsedTime*(.1 * i), 0, 0, 1);
		
		vbo.draw(GL_QUAD_STRIP, 0, numVertices );
		
		ofPopMatrix();
		
	}
	
	//end it
	dataShader.end();
	camera.end();
	fbo.end();
	
	
	//draw fbo to the screen
	ofSetColor(255, 255, 255 );
	fboShader.begin();
	fboShader.setUniform1f( "radius", radius );
	fboShader.setUniform1f( "time", elapsedTime );
	fboShader.setUniform1f( "minThreshold", minThreshold );
	fboShader.setUniform1f( "maxThreshold", maxThreshold );
	fboShader.setUniform1f( "sampleRadius", sampleRadius );
	fboShader.setUniform2f( "texDim", fbo.getWidth(), fbo.getHeight() );
	fboShader.setUniformTexture("tex", fbo.getTextureReference(0), 0 );
	fboShader.setUniformTexture("deferredPass", fbo.getTextureReference(1), 1 );
	fbo.draw(0, 0, ofGetWidth(), ofGetHeight() );
	fboShader.end();
	
	
	//GUI
	if( bHide ){
		glDisable(GL_DEPTH_TEST);
		
		fbo.getTextureReference(0).draw( ofGetWidth() - 250, 25, 200, 200 );
		fbo.getTextureReference(1).draw( ofGetWidth() - 250, 250, 200, 200 );
		
		gui.draw();
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
	presetNames[ "working" ].setValue( true );
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
