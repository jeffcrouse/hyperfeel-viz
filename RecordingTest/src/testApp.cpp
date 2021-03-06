#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    frameRate = 30;
    ofSetFrameRate(frameRate);
    ofSetVerticalSync(true);
    
    _attention.x = ofRandomf();
    _meditation.x = ofRandomf();
    
    audioLevelsPreview.allocate(320, 240);
    frame.allocate(640, 480, OF_IMAGE_COLOR);
    
    spread = 2.0;
    bRecording = false;
    cam.initGrabber(320, 240);
    
    //
    //  Set up audio
    //
    masterMixer.setInputBusCount(2);
    
    reverb = ofxAudioUnit(kAudioUnitType_Effect, kAudioUnitSubType_MatrixReverb);
    varispeed = varispeed = ofxAudioUnit(kAudioUnitType_FormatConverter, kAudioUnitSubType_Varispeed);
    lowpass = ofxAudioUnit(kAudioUnitType_Effect, kAudioUnitSubType_LowPassFilter);
    delay = ofxAudioUnit(kAudioUnitType_Effect, kAudioUnitSubType_Delay);
    distortion = ofxAudioUnit(kAudioUnitType_Effect, kAudioUnitSubType_Distortion);
    
    
//    -BrainWave01-Both.wav -- do not use
//    -BrainWave02-Med.wav - this sound can't be loud 
//    -BrainWave03-Attn.wav - to use with delay. conflicts with 07/09
//    -BrainWave04-Both.wav - this can be relatively constant
//    -BrainWave05-Attn.wav - not sure this one works. DNU for now
//    -BrainWave06-Attn.wav - sort of conflicts with sound 04. BETTER AT LOWER ATT
//    -BrainWave07-Attn.wav - sort of conflicts with sound 03. delay can be used on this as well BETTER AT HIGHER ATT
//    -BrainWave08-Med.wav  - sort of conflics with 15, 12, 13
//    -BrainWave09-Attn.wav - sort of conflicts with 07, 03. 
//    -BrainWave10-Attn.wav - MID ATTN
//    -BrainWave11-Med.wav - very calm moments 
//    -BrainWave12-Med.wav - sort of conflicts with 15
//    -BrainWave13-Med.wav - sort of conflics with 12
//    -BrainWave14-Both.wav - sort of conflicts with 9. HIGH MED
//    -BrainWave15-Med.wav  - conflicts with 12, 13, 8
//    -BRainWave16-Attn.wav - MID ATTN
//    BrainWave17-Attn.wav - this sound can't play with many other sounds (blink?)
    
    
    attention_sounds.push_back("5minsilence.wav");
    attention_sounds.push_back("sounds2/BrainWave03-Attn.wav");
    attention_sounds.push_back("sounds2/BrainWave06-Attn.wav"); // DON'T USE WITH 4
    attention_sounds.push_back("5minsilence.wav");
    attention_sounds.push_back("sounds2/BRainWave16-Attn.wav");
    attention_sounds.push_back("sounds2/BrainWave07-Attn.wav");
    attention_sounds.push_back("sounds2/BrainWave10-Attn.wav");
    attention_sounds.push_back("5minsilence.wav");
    attention_sounds.push_back("sounds2/BrainWave09-Attn.wav");
    attention_sounds.push_back("sounds2/BrainWave17-Attn.wav");
    attention_sounds.push_back("5minsilence.wav");
    
    meditation_sounds.push_back("sounds2/5minsilence.wav");
    meditation_sounds.push_back("sounds2/BrainWave15-Med.wav");
    meditation_sounds.push_back("sounds2/BrainWave11-Med.wav");
    meditation_sounds.push_back("sounds2/5minsilence.wav"); //BrainWave08-Med.wav"); // lower level
    meditation_sounds.push_back("sounds2/BrainWave04-Both.wav");
    meditation_sounds.push_back("sounds2/BrainWave12-Med.wav");
    meditation_sounds.push_back("sounds2/BrainWave14-Both.wav");
    meditation_sounds.push_back("sounds2/BrainWave13-Med.wav"); 
    meditation_sounds.push_back("sounds2/5minsilence.wav");
    
    
    attentionVolume = new float[ attention_sounds.size() ];
    attentionLoops = new ofxAudioUnitFilePlayer[ attention_sounds.size() ];
    attentionMixer.setInputBusCount(attention_sounds.size());
    attentionMixer.connectTo(masterMixer, 0);
    for(int i=0; i<attention_sounds.size(); i++) {
        string fname = attention_sounds[i];
        attentionLoops[i].setFile(ofFilePath::getAbsolutePath(fname));
        attentionLoops[i].connectTo(attentionMixer, i);
        attentionLoops[i].loop();
        attentionMixer.setInputVolume(0, i);
    }
    
    
    

    
    meditationVolume = new float[ meditation_sounds.size() ];
    meditationLoops = new ofxAudioUnitFilePlayer[ meditation_sounds.size() ];
    meditationMixer.setInputBusCount(meditation_sounds.size());
    meditationMixer.connectTo(masterMixer, 1);
    for(int i=0; i<meditation_sounds.size(); i++) {
        string fname = meditation_sounds[i];
        meditationLoops[i].setFile(ofFilePath::getAbsolutePath(fname));
        meditationLoops[i].connectTo(meditationMixer, i);
        meditationLoops[i].loop();
        meditationMixer.setInputVolume(0, i);
    }
    
    
    

    
    reverb.printParameterList();
    varispeed.printParameterList();
    lowpass.printParameterList();
    delay.printParameterList();
    distortion.printParameterList();
        
    masterMixer
//        .connectTo(reverb)
//        .connectTo(lowpass)
//        .connectTo(delay)
//        .connectTo(distortion)
//        .connectTo(varispeed)
        .connectTo(output);
    output.start();
    
    
    debugMessage    << "Press 1 for reverb settings\n"
                    << "Press 2 for varispeed settings\n"
                    << "Press 3 for lowpass settings\n"
                    << "Press 4 for delay settings\n"
                    << "Press 5 for distortion settings\n";
    
    
    AudioUnitSetParameter(reverb.getUnit(), kReverbParam_DryWetMix, kAudioUnitScope_Global, 0, 0, 0);
    AudioUnitSetParameter(varispeed.getUnit(), kVarispeedParam_PlaybackRate, kAudioUnitScope_Global, 0, 1, 0);
    AudioUnitSetParameter(lowpass.getUnit(), kLowPassParam_CutoffFrequency, kAudioUnitScope_Global, 0, 6900, 0);
    AudioUnitSetParameter(delay.getUnit(), kDelayParam_WetDryMix, kAudioUnitScope_Global, 0, 0, 0);
    AudioUnitSetParameter(distortion.getUnit(), kDistortionParam_FinalMix, kAudioUnitScope_Global, 0, 0, 0);
    
    //
    // Set up recorder
    //
    sampleRate = 44100;
    channels = 2;
    vidRecorder.setVideoCodec("mpeg4");
    vidRecorder.setVideoBitrate("800k");
    vidRecorder.setAudioCodec("mp3");
    vidRecorder.setAudioBitrate("192k");
    
    
    
    //
    //  SoundStream setup
    //
    soundStream.listDevices();
    //soundStream.setDeviceID(5);
    soundStream.setup(this, 0, channels, sampleRate, 256, 4);

    
    
    ofAddListener(httpUtils.newResponseEvent,this,&testApp::newResponse);
	httpUtils.start();
    
    
    setupUI();
}

//--------------------------------------------------------------
void testApp::exit() {
    vidRecorder.close();
}

//--------------------------------------------------------------
void testApp::setupUI()
{
		
    int columnWidth = 75;
	
	float dim = 24;
	float xInit = OFX_UI_GLOBAL_WIDGET_SPACING;
    float length = 320-xInit;
    
    ofxUICanvas* guiMain = new ofxUICanvas(columnWidth, 0, length+xInit, columnWidth);
	guiMain->setName("Main");
	guiMain->setPosition(10, 50);
	guiMain->setColorFill(ofxUIColor(200));
	guiMain->setColorFillHighlight(ofxUIColor(255));
	guiMain->setColorBack(ofxUIColor(20, 20, 20, 150));
    
    guiMain->addWidgetDown(new ofxUILabel("SETUP", OFX_UI_FONT_LARGE));
 
    guiMain->addWidgetDown( new ofxUIBaseDraws(320, 240, &audioLevelsPreview, "AUDIO LEVELS", true) );
    guiMain->addSlider("SPREAD", 1, 5, &spread );
    //guiMain->addSpacer(length-xInit, 1);
    
    //guiMain->addWidgetDown(new ofxUILabel("EFFECTS", OFX_UI_FONT_LARGE));
    //guiMain->addSpacer(length-xInit, 1);

//    guiMain->addSlider("REVERB", 1, 100, &_reverb );
//    guiMain->addSlider("DISTORTION", 1, 100, &_distortion );
//    guiMain->addSlider("COMPRESSION", 1, 100, &_compression );
//    guiMain->addSlider("DELAY", 1, 100, &_delay );
//    guiMain->addSlider("LOWPASS", 10, 6900, &_lowpass );
//    guiMain->addSlider("VARISPEED", -2, 2, &_varispeed );
    
    guiMain->autoSizeToFitWidgets();
    ofAddListener( guiMain->newGUIEvent,this, &testApp::guiEvent );
    
    guis.push_back(guiMain);
}

//--------------------------------------------------------------
void testApp::newResponse(ofxHttpResponse & response){
	cout << ofToString(response.status) + ": " + (string)response.responseBody << endl;
}

//--------------------------------------------------------------
void testApp::guiEvent(ofxUIEventArgs &e){
	
	string name = e.widget->getName();
	int kind = e.widget->getKind();
    
     if(name == "SPREAD")
    {
        ofxUIMinimalSlider *slider = (ofxUIMinimalSlider *) e.widget;
		spread = slider->getScaledValue();
        cout << "spread: " << slider->getScaledValue() << endl;

    }
}


//--------------------------------------------------------------
void testApp::update(){
    float max_volume = 1/(float)ceil(spread);
    
    _attention.x += 0.001;
    _meditation.x += 0.001;
    attention = ofNoise(_attention.x, _attention.y)*attention_sounds.size();
    meditation = ofNoise(_meditation.x, _meditation.y)*meditation_sounds.size();
    
    for(int i=0; i<attention_sounds.size(); i++) {
        float dist = fabs(attention-i);
        attentionVolume[i] = ofMap(dist, 0, spread, max_volume, 0, true);
        attentionMixer.setInputVolume(attentionVolume[i], i);
    }

    for(int i=0; i<meditation_sounds.size(); i++) {
        float dist = fabs(meditation-i);
        meditationVolume[i] = ofMap(dist, 0, spread, max_volume, 0, true);
        meditationMixer.setInputVolume(meditationVolume[i], i);
    }
    
    if(bRecording){
        frame.grabScreen(0, 0, 640, 480);
        vidRecorder.addFrame(frame.getPixelsRef());
    }
    
    
    cam.update();
    
    // 320x240
    float bar_width = 10;
    audioLevelsPreview.begin();
    {
        ofClear(0, 0, 0);        
        ofPushMatrix();
        {
            ofTranslate(10, audioLevelsPreview.getHeight());
            ofSetColor(ofColor::black);
            ofDrawBitmapString("ATTENTION", 0, -200);
            for(int i=0; i<attention_sounds.size(); i++) {
                ofSetColor(ofColor::wheat);
                ofRect(0, 20, bar_width, -ofMap(attentionVolume[i], 0, max_volume, 0, audioLevelsPreview.getHeight()));
                ofSetColor(ofColor::black);
                ofDrawBitmapString(ofToString(i), 0, 0);
                ofTranslate(bar_width+5, 0);
            }
            
            ofTranslate(bar_width+5, 0);
            ofSetColor(ofColor::black);
            ofDrawBitmapString("MEDITATION", 0, -200);
            for(int i=0; i<meditation_sounds.size(); i++) {
                ofSetColor(ofColor::wheat);
                ofRect(0, 20, bar_width, -ofMap(meditationVolume[i], 0, max_volume, 0, audioLevelsPreview.getHeight()));
                ofSetColor(ofColor::black);
                ofDrawBitmapString(ofToString(i), 0, 0);
                ofTranslate(bar_width+5, 0);
            }
        }
        ofPopMatrix();
    }
    audioLevelsPreview.end();
}

//--------------------------------------------------------------
void testApp::audioIn(float *input, int bufferSize, int nChannels){
    if(bRecording)
        vidRecorder.addAudioSamples(input, bufferSize, nChannels);
}

//--------------------------------------------------------------
void testApp::draw(){
    ofSetColor(ofColor::white);
    cam.draw(0, 0, ofGetWidth(), ofGetHeight());
    
    ofSetColor(ofColor::red);
    ofRect(0, 0, ofMap(attention, 0, attention_sounds.size(), 0, ofGetWidth()), 20);
    ofSetColor(ofColor::blue);
    ofRect(0, 20, ofMap(meditation, 0, meditation_sounds.size(), 0, ofGetWidth()), 20);
    
    ofSetColor(ofColor::white);
    ofDrawBitmapString("Attention", 10, 15);
    ofDrawBitmapString("Meditation", 10, 35);
    
    if(bRecording) {
        ofSetColor(ofColor::red, (0.5+sin(ofGetElapsedTimef()*5)*0.5)*255);
        ofCircle(ofGetWidth()-20, 20, 10);
    }
    
    ofSetColor(ofColor::white);
    ofDrawBitmapString(debugMessage.str(), ofGetWidth()-300, ofGetHeight()-80);
}


//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

    if(key=='r') {
        if(bRecording) {
            vidRecorder.close();
            bRecording = false;
            
            ofxHttpForm form;
            form.action = "http://cheese.local:3000/submit/video";
            form.method = OFX_HTTP_POST;
            form.addFormField("journey_id", journey_id);
            form.addFormField("email", "jeff@crouse.cc");
            form.addFile("video", filename);
            httpUtils.addForm(form);
            
        } else {
            journey_id = ofGetTimestampString("%m%d%H%M%S");
            filename = "videos/"+journey_id+".mov";
            
            vidRecorder.setup(filename, frame.getWidth(), frame.getHeight(), frameRate, sampleRate, channels);
            bRecording = true;
        }
    }
    
	if( key == 'h' || key == 'H' ){
		for(vector<ofxUICanvas *>::iterator it = guis.begin(); it != guis.end(); ++it) {
			(*it)->toggleVisible();
		}
	}
    
    if(key=='1') {
        reverb.showUI();
    } else if(key=='2') {
        varispeed.showUI();
    } else if(key=='3') {
        lowpass.showUI();
    } else if(key=='4') {
        delay.showUI();
    } else if(key=='5') {
        distortion.showUI();
    }
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

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
