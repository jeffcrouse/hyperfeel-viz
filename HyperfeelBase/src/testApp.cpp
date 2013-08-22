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
	
	fboMap.allocate( 1024, 1024, GL_RGBA, 4 );
	
	cout << "fbo.getNumTextures(): " << fbo.getNumTextures() << endl;
	//shaders
	loadShaders();
	
	//make some fake data
	vector<HyperFeel_Data> data = getFakeData();
	
	drawType = "displacedMesh";//"rainbowLayers";
	bRainbowLayersIsSetup = bDisplacedMeshIsSetup = false;
	
	
	bRussianDollsAreSetup = false;
	
	setDefaults();
	
    // ---
	setupUI();

}


void testApp::setDefaults(){
	goboRadius = ofGetWidth()/2;
	
	uiCurveWidth = 100, uiCurveRadius = 500, uiCurveOffset = 100;
	
	uiMinThreshold = .002;
	uiMaxThreshold = .01;
	uiSampleRadius = 50.;
	uiNearClip = 1, uiFarClip = 10000;
	
	uiDisplacement = 400;
	uiDeltaExpo = 4.;
	uiNoiseSurfaceSampleScale = .03;
	uiRoundingWeight = .8;
	frExpo = 2;
	
	camera.setFov( 60 );
	camera.setNearClip( 50 );
	camera.setFarClip( 5000 );
	
	uiRussianDollsAlpha = 1.;
}


void testApp::setupUI(){
	
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
	guiMain->addSlider("goboRadius", 256, 1024, &goboRadius );
	
	//create a radio for switching renderTypes
	guiMain->addSpacer();
	guiMain->addLabel("Effects" );
	guiMain->addRadio("render types", renderTypes );
	
	guiMain->autoSizeToFitWidgets();
	
	
	
	//get our presets and them to the radio
	presetGui = new ofxUICanvas();
	
	//save button
	presetGui->addSpacer();
	presetGui->addButton("save preset", false);
	presetGui->addSpacer();
	
	presetGui->setName("PRESETS");
	presetGui->addLabel("PRESETS");
	presetGui->setPosition( guiMain->getRect()->getX(), guiMain->getRect()->getY() + guiMain->getRect()->getHeight() + 30);
	presetGui->addSpacer();
	presetRadio = presetGui->addRadio("presets", getPresetNames(), false );
	
	presetGui->autoSizeToFitWidgets();

	
	//add some effect specific gui stuff
	ofxUICanvas* rainbowLayersGui = new ofxUICanvas();
	rainbowLayersGui->addLabel("rainbow layers" );
	rainbowLayersGui->addSpacer();
	rainbowLayersGui->setName("rainbowLayersGui");
	rainbowLayersGui->setPosition( guiMain->getRect()->getX() + guiMain->getRect()->getWidth() + 30, guiMain->getRect()->getY() );
	
	//uiMinThreshold = .02, uiMaxThreshold = .2, uiSampleRadius = 500;
	rainbowLayersGui->addLabel("ssao");
	rainbowLayersGui->addSlider("sampleRadius", 1., 1000., &uiSampleRadius );
	rainbowLayersGui->addSlider("minThreshold", .001, .1, &uiMinThreshold );
	rainbowLayersGui->addSlider("maxThreshold", .1, .5, &uiMaxThreshold );
	rainbowLayersGui->addSlider("nearClip", 1., 50, &uiNearClip );
	rainbowLayersGui->addSlider("farClip", 50, 5000, &uiFarClip );
	
	
	//uiCurveOffset, uiCurveRadius, uiCurveWidth;
	rainbowLayersGui->addLabel("mesh settings");
	rainbowLayersGui->addSlider("ribbon offset", -500, 500, &uiCurveOffset );
	rainbowLayersGui->addSlider("ribbon width", 1, 1000, &uiCurveWidth );
	rainbowLayersGui->addSlider("ribbon radius", 100, 1000, &uiCurveRadius );
	
	rainbowLayersGui->autoSizeToFitWidgets();
	guis.push_back(rainbowLayersGui);
	
	displacedMeshGui = new ofxUICanvas();
	displacedMeshGui->setName("displacedMesh");
	displacedMeshGui->setPosition(rainbowLayersGui->getRect()->getX() + rainbowLayersGui->getRect()->getWidth() + 30, rainbowLayersGui->getRect()->getY() );
	
	displacedMeshGui->addLabel("Displaced Mesh");
	displacedMeshGui->addSpacer();
	displacedMeshGui->addSlider("displacement", -1000, 1000, &uiDisplacement );
	displacedMeshGui->addSlider("deltaExpo", 1., 32., &uiDeltaExpo );
	displacedMeshGui->addSlider("noiseSurfaceSampleScale", .0001, .3, &uiNoiseSurfaceSampleScale );
	displacedMeshGui->addSlider("roundingWeight", .001, 1.000, &uiRoundingWeight );
	displacedMeshGui->addSlider("frExpo", 1., 32.000, &frExpo );
	
	displacedMeshGui->addSpacer();
	displacedMeshGui->addLabel("Shaders");
	displacedMeshGui->addSpacer();
	
	displacedMeshGui->addRadio("shaders", dispShaderNames );

	displacedMeshGui->autoSizeToFitWidgets();
	
	russianDollGui = new ofxUICanvas();
	russianDollGui->setName("russianDolls");
	russianDollGui->addSpacer();
	russianDollGui->setPosition( rainbowLayersGui->getRect()->getX(), rainbowLayersGui->getRect()->getY() + rainbowLayersGui->getRect()->getHeight() + 20 );
	
	russianDollGui->addLabel("Russian Dolls");
	russianDollGui->addLabel( "uniforms" );
	russianDollGui->addSpacer();
	russianDollGui->addSlider("alpha", 0, 1, &uiRussianDollsAlpha );
	
	
	
	
	//add listeners
	
	guis.push_back(displacedMeshGui);
	guis.push_back( guiMain  );
	guis.push_back( presetGui );
	guis.push_back( russianDollGui );
	
	bPresetsloadedHack = false;
	ofAddListener( displacedMeshGui->newGUIEvent,this,&testApp::guiEvent );
	ofAddListener( guiMain->newGUIEvent,this,&testApp::guiEvent );
	ofAddListener( presetGui->newGUIEvent,this,&testApp::guiEvent );
	
	
	
	
	//probably don;t need this but it's here.
	dispShaderName = dispShaderNames[0];
	
	//load our working sttings
//	loadPreset("Working");
	nextPreset = "Working";
}

void testApp::guiEvent(ofxUIEventArgs &e){
	
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
			for(int i=0; i<dispShaderNames.size(); i++){
				if(name == dispShaderNames[i]){
					if(e.getToggle()->getValue()){
						dispShaderName = dispShaderNames[i];
//						bFoundIt = true;
					}
				}
			}
		}
	}
	
}

void testApp::guiPresetEvent(ofxUIEventArgs &e)
{
	cout << "guiPresetEvent"<<endl;
}

//--------------------------------------------------------------
void testApp::loadShaders()
{
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
	dispShaderNames.push_back("displacedNoise");
	displacedShader.load( "shaders/displaced" );
	
	disp_1.load( "shaders/disp_1" );
	dispShaderNames.push_back("dispShader_1");
	
	disp_2.load( "shaders/disp_2" );
	dispShaderNames.push_back("dispShader_2");
	
	disp_3.load( "shaders/disp_3" );
	dispShaderNames.push_back("dispShader_3");
	
	disp_4.load( "shaders/disp_4" );
	dispShaderNames.push_back("dispShader_4");
	
	
	
	dollShader.load( "shaders/doll" );
	
}


//--------------------------------------------------------------
void testApp::update()
{
	elapsedTime = ofGetElapsedTimef();
	if(currentPresetName != nextPreset){
		loadPreset( nextPreset );
	}
	if(bSavePreset){
		savePreset();
	}
}

//--------------------------------------------------------------
void testApp::draw()
{
	//
	ofBackground( 0,0,0 );
	
	glEnable(GL_DEPTH_TEST);
	
	if(currentRenderType == "rainbowLayers" ){
		drawRainbowLayers();
	}
	else if( currentRenderType == "displacedMesh" ){
		drawDisplacedMesh();
	}
	else if( currentRenderType == "russianDolls" ){
		drawRussianDolls();
	}
	
	
	glDisable(GL_DEPTH_TEST);
	
	if( guis[0]->isVisible() && currentRenderType == "displacedMesh" && dispShaderName == "dispShader_4"){
		ofSetColor(255, 255, 255, 255);
		fboMap.draw(10, ofGetHeight() - 210, 200, 200);
	}
	
}

void testApp::drawRussianDolls(){
	if(!bRussianDollsAreSetup){
		cout << "setting up russian dolls" << endl;
		setupRussianDolls();
	}
	
	//update
	for(int i=0; i<dollNodes.size(); i++){
		if( i == 0){
			dollNodes[i].setOrientation(ofVec3f( 0, sin(elapsedTime * .4)*2, sin(elapsedTime * .2)*10. ));
		}else{
			
			dollNodes[i].setOrientation( dollNodes[i-1].getOrientationEuler() );
		}
	}
	
	
	//draw
	ofPushStyle();
	ofBlendMode( OF_BLENDMODE_ADD );
	glEnable(GL_CULL_FACE);
	camera.begin();
	
	dollShader.begin();
	dollShader.setUniform1f("alpha", uiRussianDollsAlpha );
	for(int i=dollNodes.size()-1; i>=0; i--){
		ofPushMatrix();
		ofMultMatrix( dollNodes[i].getGlobalTransformMatrix() );
		ofSetColor( dollColors[i] );
		
		glCullFace(GL_BACK);
		dollVbo.drawElements(GL_QUADS, russianDallIndexCount );
		
		glCullFace(GL_FRONT);
		dollVbo.drawElements(GL_QUADS, russianDallIndexCount );
		
		ofPopMatrix();
	}
	
	dollShader.end();
	
	camera.end();
	ofPopStyle();
	glDisable(GL_CULL_FACE);
	
	
}

void testApp::setupRussianDolls( float radians, float sphereRad ){
	bRussianDollsAreSetup = true;
	
	int subdX = 64, subdY = 32;
	int vertCount = subdX * subdY;
	
	//make our vertices
	vector<ofVec3f> vertices(vertCount);
	vector<ofVec3f> normals(vertCount);
	vector<ofVec2f> texCoords(vertCount);
	int count = 0;
	float xStep = 1. / float(subdX-1);
	float yStep = 1. / float(subdY-1);
	float zStep = -subdY / 2;
	
	float xval, yval;
	for (int i=0; i<subdY; i++) {
		for (int j=0; j<subdX; j++) {
			//make our vertices
			
			xval = xStep*j*TWO_PI;
			yval = yStep*i * radians;
			float u = xval;
			float v = yval;
			
			vertices[count].set( cos(u)*sin(v), sin(u)*sin(v), -cos(v) );
			normals[count] = vertices[count];
			
			vertices[count] *= sphereRad;
			
			texCoords[count].set( j * xStep, i * yStep );
			count++;
		}
	}
	
	
	//make the faces, calculate the face normals and add them to the vertex normals
	vector<ofIndexType> indices;
	int p0, p1, p2, p3, wrapIndex;
	for (int i=1; i<subdY; i++) {
		for (int j=1; j<subdX; j++) {
			
			//quad faces
			p0 = (i-1) * subdX + j-1;
			p1 = i*subdX + j-1;
			p2 = i*subdX + j;
			p3 = (i-1) * subdX + j;
			
			indices.push_back( p0 );
			indices.push_back( p1 );
			indices.push_back( p2 );
			indices.push_back( p3 );
		}
	}
	russianDallIndexCount = indices.size();
	
	
	//add attributes to vbo
	dollVbo.setVertexData( &vertices[0], vertices.size(), GL_STATIC_DRAW );
	dollVbo.setNormalData( &normals[0], normals.size(), GL_STATIC_DRAW );
	dollVbo.setTexCoordData( &texCoords[0], texCoords.size(), GL_STATIC_DRAW );
	dollVbo.setIndexData( &indices[0], indices.size(), GL_STATIC_DRAW );
	
	
	//create our nodes
	dollNodes.resize( 30 );
	dollColors.resize( dollNodes.size() );
	
	for(int i=0; i<dollNodes.size(); i++){
		dollColors[i].set( ofRandom(.5,1.5), ofRandom(.5,1.5), ofRandom(.5,1.5) );
		dollNodes[i].setScale(.9);
		
		if(i>0){
			dollNodes[i].setParent( dollNodes[i-1] );
		}
	}
}


//--------------------------------------------------------------
void testApp::setupDisplacedMesh( float radians, float sphereRad )
{
	
	bDisplacedMeshIsSetup = true;
	cout << endl << "setting up displacedMesh" << endl << endl;
	
	//setup shader
	displacedShader.load( "shaders/displaced" );
	
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
			
			vertices[count].set( cos(u)*sin(v), sin(u)*sin(v), -cos(v) );
			
			vertices[count] *= sphereRad;
			
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

void testApp::drawDisplacedMesh()
{
	if(!bDisplacedMeshIsSetup){
		setupDisplacedMesh();
	}
	
	
	camera.begin();
	
	ofPushMatrix();
	ofScale(1, 1, 3);
	

	if( dispShaderName == "dispShader_1"){
		disp_1.begin();
		
		disp_1.setUniform1f("time", elapsedTime );
		disp_1.setUniform1f("nearClip", uiNearClip );
		disp_1.setUniform1f("farClip", uiFarClip );
		disp_1.setUniform1f("displacement", uiDisplacement );
		disp_1.setUniform1f("deltaExpo", uiDeltaExpo );
		disp_1.setUniform1f("frExpo", frExpo );
		disp_1.setUniform1f("noiseSurfaceSampleScale", uiNoiseSurfaceSampleScale );
	}
	else if( dispShaderName == "dispShader_2"){
		disp_2.begin();
		
		disp_2.setUniform1f("time", elapsedTime );
		disp_2.setUniform1f("nearClip", uiNearClip );
		disp_2.setUniform1f("farClip", uiFarClip );
		disp_2.setUniform1f("displacement", uiDisplacement );
		disp_2.setUniform1f("deltaExpo", uiDeltaExpo );
		disp_2.setUniform1f("frExpo", frExpo );
		disp_2.setUniform1f("noiseSurfaceSampleScale", uiNoiseSurfaceSampleScale );
	}
	
	else if( dispShaderName == "dispShader_3"){
		disp_3.begin();
		disp_3.setUniform1f("time", elapsedTime );
		disp_3.setUniform1f("nearClip", uiNearClip );
		disp_3.setUniform1f("farClip", uiFarClip );
		disp_3.setUniform1f("displacement", uiDisplacement );
		disp_3.setUniform1f("deltaExpo", uiDeltaExpo );
		disp_3.setUniform1f("frExpo", frExpo );
		disp_3.setUniform1f("noiseSurfaceSampleScale", uiNoiseSurfaceSampleScale );
	}
	
	else if( dispShaderName == "dispShader_4"){
		fboMap.begin();
		
		ofClear(10,0,0,0);
		
		int numBoxes = 64;
		float step = 1024 / numBoxes;
		for (int j=-1; j<2; j++) {
			ofPushMatrix();
			ofTranslate(fboMap.getWidth() * j, 0);
			ofSeedRandom();
			for (int i=0; i<=numBoxes; i++) {
				
				ofSetColor( 255,255,255  );
				
				ofPushMatrix();
				ofTranslate(step * i, fboMap.getHeight()/2, 0);
				ofRotate(elapsedTime * 3. + i*step, 0, 0, 1 );
				ofDrawBox(0,0,0, 6, fboMap.getHeight(), 6);
				ofPopMatrix();
			}
			
			ofPopMatrix();
		}
		
		fboMap.end();
		
		disp_4.begin();
		disp_4.setUniform1f("time", elapsedTime );
		disp_4.setUniform1f("nearClip", uiNearClip );
		disp_4.setUniform1f("farClip", uiFarClip );
		disp_4.setUniform1f("displacement", uiDisplacement );
		disp_4.setUniform1f("deltaExpo", uiDeltaExpo );
		disp_4.setUniform1f("frExpo", frExpo );
		disp_4.setUniform1f("noiseSurfaceSampleScale", uiNoiseSurfaceSampleScale );
		disp_4.setUniformTexture("tex", fboMap.getTextureReference(), 0);
		disp_4.setUniform2f("texDim", fboMap.getWidth(), fboMap.getHeight() );
	}
	else//if(dispShaderName == "displacedMesh")
	{
		
		displacedShader.begin();
		displacedShader.setUniform1f("time", elapsedTime );
		displacedShader.setUniform1f("nearClip", uiNearClip );
		displacedShader.setUniform1f("farClip", uiFarClip );
		displacedShader.setUniform1f("displacement", uiDisplacement );
		displacedShader.setUniform1f("deltaExpo", uiDeltaExpo );
		displacedShader.setUniform1f("frExpo", frExpo );
		displacedShader.setUniform1f("noiseSurfaceSampleScale", uiNoiseSurfaceSampleScale );
		
	}
	
	
	
//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE );
	displacedVbo.drawElements(GL_TRIANGLES, displacedIndexCount );
//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL );
	

	if( dispShaderName == "dispShader_1")
	{
		disp_1.end();
	}
	else if( dispShaderName == "dispShader_2")
	{
		disp_2.end();
	}
	
	else if( dispShaderName == "dispShader_3")
	{
		disp_3.end();
	}
	
	else if( dispShaderName == "dispShader_4")
	{
		disp_4.end();
	}
	else//	if(dispShaderName == "displacedMesh")
	{
		displacedShader.end();
	}
	
	ofPopMatrix();
	
	camera.end();
	
}

ofVec3f testApp::normalFrom3Points(ofVec3f p0, ofVec3f p1, ofVec3f p2)
{
	ofVec3f norm = (p2 - p1).cross( p0 - p1);
	return norm.normalized();
}

ofVec3f testApp::normalFrom4Points(ofVec3f p0, ofVec3f p1, ofVec3f p2, ofVec3f p3)
{
	return (normalFrom3Points(p0, p1, p2) + normalFrom3Points(p0, p2, p3)).normalized();
}


//--------------------------------------------------------------
void testApp::setupRainbowLayers()
{
	bRainbowLayersIsSetup = true;
	
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

void testApp::drawRainbowLayers()
{
	//Draw out scene to the fbo
	fbo.begin();
    fbo.activateAllDrawBuffers();
	ofClear(0,0,0,255);
	//    ofBackgroundGradient( bckgrnd0, bckgrnd1 );
	
	//set camera attributes
	//	camera.setFov( uiFov );
	//	camera.setNearClip( nearClip );
	//	camera.setFarClip( farClip );
	
	//draw the data
	camera.begin();
	
	
	
	if( !bRainbowLayersIsSetup ){
		setupRainbowLayers();
	}
	
	dataShader.begin();
	dataShader.setUniform1f("nearClip", uiNearClip );
	dataShader.setUniform1f("farClip", uiFarClip );
	dataShader.setUniform1f("curveWidth", uiCurveWidth );
	dataShader.setUniform1f( "time", elapsedTime );
	ofColor c;
	for (int i=0; i < 30; i++) {
		
		c.setHsb(255 * float(i)/30, 150, 255);
		
		dataShader.setUniform3f("color", float(c.r)/255., float(c.g)/255., float(c.b)/255. );
		dataShader.setUniform1f("radius", uiCurveRadius * (1. - (float(i)/30)*.98) );
		dataShader.setUniform1f("offset", uiCurveOffset * (1. - (float(i)/30)*.98) );
		
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
	fboShader.setUniform1f( "radius", goboRadius );
	fboShader.setUniform1f( "time", elapsedTime );
	fboShader.setUniform1f( "minThreshold", uiMinThreshold );
	fboShader.setUniform1f( "maxThreshold", uiMaxThreshold );
	fboShader.setUniform1f( "sampleRadius", uiSampleRadius );
	fboShader.setUniform2f( "texDim", fbo.getWidth(), fbo.getHeight() );
	fboShader.setUniformTexture("tex", fbo.getTextureReference(0), 0 );
	fboShader.setUniformTexture("deferredPass", fbo.getTextureReference(1), 1 );
	fbo.draw(0, 0, ofGetWidth(), ofGetHeight() );
	fboShader.end();
	
	
}


//--------------------------------------------------------------
void testApp::exit()
{
	
	savePreset("Working");
	
	for(int i=0; i<guis.size(); i++){
		
		ofRemoveListener(guis[i]->newGUIEvent,this,&testApp::guiEvent);
		delete guis[i];
	}
	
	for(int i=0; i<presetEventGuis.size(); i++){
		
		ofRemoveListener(presetEventGuis[i]->newGUIEvent,this,&testApp::guiPresetEvent);
		delete presetEventGuis[i];
	}
}

//--------------------------------------------------------------
vector<string> testApp::getPresetNames()
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

void testApp::loadPreset( string presetName)
{
	currentPresetName = presetName;
	
	for(int i = 0; i < guis.size(); i++)
    {
		cout << "Presets/" + presetName + "/"+guis[i]->getName()+".xml" << endl;
        guis[i]->loadSettings( "Presets/" + presetName + "/"+guis[i]->getName()+".xml");
		
    }
}

void testApp::savePreset( string presetName )
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
    
    for(int i = 0; i < guis.size(); i++)
    {
		cout << "saving: " << presetDirectory + guis[i]->getName()+".xml" << endl;
        guis[i]->saveSettings(presetDirectory + guis[i]->getName()+".xml");
    }
}


//--------------------------------------------------------------
void testApp::keyPressed(int key)
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
}
//--------------------------------------------------------------
void testApp::keyReleased(int key)
{
	
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y )
{
	
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button)
{
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{
	
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{
	
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
