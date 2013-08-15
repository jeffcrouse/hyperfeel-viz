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
	
	fbo.createAndAttachTexture(GL_RGBA32F, 1);
	
	cout << "fbo.getNumTextures(): " << fbo.getNumTextures() << endl;
	//shaders
	loadShaders();
	
	//make some fake data
	vector<HyperFeel_Data> data = getFakeData();
	
	//camera
//	camera.setNearClip(1);
//	camera.setFarClip(500);
	camera.setFov( 60 );
	
	drawType = "displacedMesh";//"rainbowLayers";
	bRainbowLayersIsSetup = bDisplacedMeshIsSetup = false;
	
	
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
	
	//displacedMesh
	displacedShader.load( "shaders/displaced" );
	
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
	
	if(drawType == "rainbowLayers"){
		drawRainbowLayers();
	}
	else if( drawType == "displacedMesh" ){
		drawDisplacedMesh();
	}
	
	camera.end();
	fbo.end();
	
	
	//draw fbo to the screen
	ofSetColor(255, 255, 255 );
//	fboShader.begin();
	fboShader.setUniform1f( "radius", radius );
	fboShader.setUniform1f( "time", elapsedTime );
	fboShader.setUniform1f( "minThreshold", minThreshold );
	fboShader.setUniform1f( "maxThreshold", maxThreshold );
	fboShader.setUniform1f( "sampleRadius", sampleRadius );
	fboShader.setUniform2f( "texDim", fbo.getWidth(), fbo.getHeight() );
	fboShader.setUniformTexture("tex", fbo.getTextureReference(0), 0 );
	fboShader.setUniformTexture("deferredPass", fbo.getTextureReference(1), 1 );
	fbo.draw(0, 0, ofGetWidth(), ofGetHeight() );
//	fboShader.end();
	
	
	//GUI
	if( bHide ){
		glDisable(GL_DEPTH_TEST);
		
		fbo.getTextureReference(0).draw( ofGetWidth() - 250, 25, 200, 200 );
		fbo.getTextureReference(1).draw( ofGetWidth() - 250, 250, 200, 200 );
		
		gui.draw();
	}
}
void testApp::setupDisplacedMesh(){
	
	bDisplacedMeshIsSetup = true;
	cout << endl << "setting up displacedMesh" << endl << endl;
	
	//setup shader
	displacedShader.load( "shaders/displaced" );
	
	//create mesh tube
	float halfsphereRad = 700;
	
	int subdX = 128, subdY = 64;//128*128 ~= 16000 * (pos+norm+tangent+bi-tangent+uv+indices) == a lot of data
	
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
			
//			vertices[count].set( sin(xStep*j*TWO_PI) * rad, cos(xStep*j*TWO_PI) * rad, i * zStep );
			
			xval = xStep*j*TWO_PI;
			yval = yStep*i*HALF_PI;
			float u = xval;
			float v = yval;
			
			vertices[count].set( cos(u)*sin(v), sin(u)*sin(v), -cos(v) );
//			vertices[count].set( cos(u)*sin(v), cos(v), sin(u)*sin(v) );
			
			vertices[count] *= halfsphereRad;
			
			texCoords[count].set( j * xStep, i * yStep );
			count++;
		}
	}
	displacedVertexCount = count;
	
	//make the faces, calculate the face normals and add them to the vertex normals
	vector<ofIndexType> indices;
	ofVec3f faceNorm;
	int p0, p1, p2, p3, wrapIndex;
	for (int i=1; i<subdY; i++) {
		for (int j=1; j<subdX; j++) {
			//we need to connect the fron to the back
//			wrapIndex = ( j == subdX-1 )? 0 : j-1;
			
			//triangle faces
			p0 = (i-1) * subdX + j-1;
			p1 = i*subdX + j-1;
			p2 = i*subdX + j;
			p3 = (i-1) * subdX + j;
			
			faceNorm = normalFrom4Points( vertices[p0], vertices[p1], vertices[p2], vertices[p3] );
			
			normals[p0] += faceNorm;
			normals[p1] += faceNorm;
			normals[p2] += faceNorm;
			normals[p3] += faceNorm;
			
			indices.push_back( p0 );
			indices.push_back( p1 );
			indices.push_back( p2 );

			indices.push_back( p0 );
			indices.push_back( p2 );
			indices.push_back( p3 );
		}
	}
	displacedIndexCount = indices.size();
	
	//wrap the normals around
	for (int i=0; i<subdY; i++) {
		p0 = i*subdX;
		p1 = i*subdX + subdX-1;
		normals[p0] = normals[p1] = (normals[p0] + normals[p1]) * .5;
	}
	
	//smooth the normals
	for (int i=0; i<normals.size(); i++) {
		normals[i].normalize();
	}
	
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
	displacedVbo.setVertexData( &vertices[0], vertices.size(), GL_STATIC_DRAW );
	displacedVbo.setNormalData( &normals[0], normals.size(), GL_STATIC_DRAW );
	displacedVbo.setTexCoordData( &texCoords[0], texCoords.size(), GL_STATIC_DRAW );
	displacedVbo.setIndexData( &indices[0], indices.size(), GL_STATIC_DRAW );
	
	displacedShader.begin();
	
	displacedVbo.setAttributeData( displacedShader.getAttributeLocation( "tangent" ), &tangents[0].x, 3, tangents.size()*3, GL_STATIC_DRAW, sizeof(tangents[0]) );
	displacedVbo.setAttributeData( displacedShader.getAttributeLocation( "binormal" ), &binormals[0].x, 3, binormals.size()*3, GL_STATIC_DRAW, sizeof(binormals[0])  );
	
	displacedShader.end();
	
	//gui switch to displaced panel...
	
}
void testApp::drawDisplacedMesh(){
	if(!bDisplacedMeshIsSetup){
		setupDisplacedMesh();
	}
	
	ofPushMatrix();
	ofScale(1, 1, 3);
	
	displacedShader.begin();
	displacedShader.setUniform1f("time", elapsedTime );
	
//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE );
	displacedVbo.drawElements(GL_TRIANGLES, displacedIndexCount );
//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL );
	
	displacedShader.end();
	
	ofPopMatrix();
	
}

ofVec3f testApp::normalFrom3Points(ofVec3f p0, ofVec3f p1, ofVec3f p2){
	
//	faces[index].normal = (vertices[faces[index][2]] - vertices[faces[index][1]]).cross( vertices[faces[index][0]] - vertices[faces[index][1]] ).normalize();
	ofVec3f norm = (p2 - p1).cross( p0 - p1);
	return norm.normalized();
}
ofVec3f testApp::normalFrom4Points(ofVec3f p0, ofVec3f p1, ofVec3f p2, ofVec3f p3){
	return (normalFrom3Points(p0, p1, p2) + normalFrom3Points(p0, p2, p3)).normalized();
}

void testApp::setupRainbowLayers(){
	bool bRainbowLayersIsSetup = true;
	
	cout << endl << "setting up rainbowLayers" << endl << endl;
	
	//make some fake data
	vector<HyperFeel_Data> data = getFakeData();
	
	//create vertex data
	numVertices = data.size() * 2;
	vector<ofVec3f> vertices( numVertices );
	
	for (int i=0; i<numVertices; i+=2	) {
		vertices[i].set( data[i/2].attention, data[i/2].timeStamp, -1 );
		vertices[i+1].set( data[i/2].meditation, data[i/2].timeStamp, 1 );
	}
	
	//close the ribbon
	vertices.push_back( ofVec3f( data[0].attention, data[0].timeStamp, -1 ) );
	vertices.push_back( ofVec3f( data[0].meditation, data[0].timeStamp, 1 ) );
	numVertices += 2;
	
	//set the vbo.. we need to use vertices or else ofVbo won't draw...
	vbo.setVertexData( &vertices[0], vertices.size(), GL_STATIC_DRAW );

}


void testApp::drawRainbowLayers(){
	
	if( !bRainbowLayersIsSetup ){
		setupRainbowLayers();
	}
	
	dataShader.begin();
	dataShader.setUniform1f("nearClip", nearClip );
	dataShader.setUniform1f("farClip", farClip );
	dataShader.setUniform1f("curveWidth", curveWidth );
	dataShader.setUniform1f( "time", elapsedTime );
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
