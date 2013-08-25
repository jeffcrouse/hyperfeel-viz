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
    
    spread = 1.5;
    bRecording = false;
    cam.initGrabber(320, 240);
    
    //
    //  Set up audio
    //
    masterMixer.setInputBusCount(2);

    attentionMixer.setInputBusCount(N_ATTENTION_LOOPS);
    attentionMixer.connectTo(masterMixer, 0);
    for(int i=0; i<N_ATTENTION_LOOPS; i++) {
        ostringstream fname;
        fname << "sounds/Att" << std::setfill('0') << std::setw(2) << (i+1) << ".wav";
        attentionLoops[i].setFile(ofFilePath::getAbsolutePath(fname.str()));
        attentionLoops[i].connectTo(attentionMixer, i);
        attentionLoops[i].loop();
        attentionMixer.setInputVolume(0, i);
    }
    
    meditationMixer.setInputBusCount(N_MEDITATION_LOOPS);
    meditationMixer.connectTo(masterMixer, 1);
    for(int i=0; i<N_MEDITATION_LOOPS; i++) {
        ostringstream fname;
        fname << "sounds/Med" << std::setfill('0') << std::setw(2) << (i+1) << ".wav";
        meditationLoops[i].setFile(ofFilePath::getAbsolutePath(fname.str()));
        meditationLoops[i].connectTo(meditationMixer, i);
        meditationLoops[i].loop();
        meditationMixer.setInputVolume(0, i);
    }
    
    
    
    reverb = ofxAudioUnit(kAudioUnitType_Effect, kAudioUnitSubType_MatrixReverb);
    varispeed = varispeed = ofxAudioUnit(kAudioUnitType_FormatConverter, kAudioUnitSubType_Varispeed);
    lowpass = ofxAudioUnit(kAudioUnitType_Effect, kAudioUnitSubType_LowPassFilter);
    //compressor = ofxAudioUnit(kAudioUnitType_Effect, kAudioUnitSubType_DynamicsProcessor);
    delay = ofxAudioUnit(kAudioUnitType_Effect, kAudioUnitSubType_Delay);
    distortion = ofxAudioUnit(kAudioUnitType_Effect, kAudioUnitSubType_Distortion);
    
    reverb.printParameterList();
    varispeed.printParameterList();
    lowpass.printParameterList();
    //compressor.printParameterList();
    delay.printParameterList();
    distortion.printParameterList();
        
    masterMixer
        .connectTo(reverb)
        .connectTo(lowpass)
        //.connectTo(compressor)
        .connectTo(delay)
        .connectTo(distortion)
        .connectTo(varispeed)
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
    soundStream.setDeviceID(5);
    soundStream.setup(this, 0, channels, sampleRate, 256, 4);


    
    
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
	guiMain->setPosition(10, 40);
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
void testApp::guiEvent(ofxUIEventArgs &e){
	
	string name = e.widget->getName();
	int kind = e.widget->getKind();
    
    /*
    if(name == "REVERB")
	{
		ofxUIMinimalSlider *slider = (ofxUIMinimalSlider *) e.widget;
        cout << "REVERB: " << slider->getScaledValue() << endl;
		_reverb = slider->getScaledValue();
    }
    else if(name=="DISTORTION")
    {
        ofxUIMinimalSlider *slider = (ofxUIMinimalSlider *) e.widget;
        cout << "DISTORTION: " << slider->getScaledValue() << endl;
        _distortion = slider->getScaledValue();
    }
    else if(name=="COMPRESSION")
    {
       ofxUIMinimalSlider *slider = (ofxUIMinimalSlider *) e.widget;
        cout << "COMPRESSION: " << slider->getScaledValue() << endl;
        _compression = slider->getScaledValue();
    }
    else if(name=="DELAY")
    {
        ofxUIMinimalSlider *slider = (ofxUIMinimalSlider *) e.widget;
        cout << "DELAY: " << slider->getScaledValue() << endl;
        _delay = slider->getScaledValue();
    }
    else if(name=="LOWPASS")
    {
        ofxUIMinimalSlider *slider = (ofxUIMinimalSlider *) e.widget;
        cout << "LOWPASS: " << slider->getScaledValue() << endl;
        _lowpass = slider->getScaledValue();
        AudioUnitSetParameter(lowpass.getUnit(),
                              kLowPassParam_CutoffFrequency,
                              kAudioUnitScope_Global,
                              0,
                              _lowpass,
                              0);
    }
    else if(name=="VARISPEED")
    {
        ofxUIMinimalSlider *slider = (ofxUIMinimalSlider *) e.widget;
        cout << "VARISPEED: " << slider->getScaledValue() << endl;
        _varispeed = slider->getScaledValue();
        AudioUnitSetParameter(varispeed.getUnit(),
                              kVarispeedParam_PlaybackRate,
                              kAudioUnitScope_Global,
                              0,
                              _varispeed,
                              0);
    }
    else 
     */
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
    
    _attention.x += 0.002;
    _meditation.x += 0.002;
    attention = ofNoise(_attention.x, _attention.y)*N_ATTENTION_LOOPS;
    meditation = ofNoise(_meditation.x, _meditation.y)*N_MEDITATION_LOOPS;
    
    for(int i=0; i<N_ATTENTION_LOOPS; i++) {
        float dist = fabs(attention-i);
        attentionVolume[i] = ofMap(dist, 0, spread, max_volume, 0, true);
        attentionMixer.setInputVolume(attentionVolume[i], i);
    }

    for(int i=0; i<N_MEDITATION_LOOPS; i++) {
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
            for(int i=0; i<N_ATTENTION_LOOPS; i++) {
                ofSetColor(ofColor::wheat);
                ofRect(0, 20, bar_width, -ofMap(attentionVolume[i], 0, max_volume, 0, audioLevelsPreview.getHeight()));
                ofSetColor(ofColor::black);
                ofDrawBitmapString(ofToString(i), 0, 0);
                ofTranslate(bar_width+5, 0);
            }
            
            ofTranslate(bar_width+5, 0);
            ofSetColor(ofColor::black);
            ofDrawBitmapString("MEDITATION", 0, -200);
            for(int i=0; i<N_MEDITATION_LOOPS; i++) {
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
    ofRect(0, 0, ofMap(attention, 0, N_ATTENTION_LOOPS, 0, ofGetWidth()), 20);
    ofSetColor(ofColor::blue);
    ofRect(0, 20, ofMap(meditation, 0, N_MEDITATION_LOOPS, 0, ofGetWidth()), 20);
    
    ofSetColor(ofColor::white);
    ofDrawBitmapString("Attention", 10, 15);
    ofDrawBitmapString("Meditation", 10, 35);
    
    if(bRecording) {
        ofSetColor(ofColor::red, (0.5+sin(ofGetElapsedTimef()*5)*0.5)*255);
        ofCircle(ofGetWidth()-20, 20, 10);
    }
    
    ofSetColor(ofColor::white);
    ofDrawBitmapString(debugMessage.str(), ofGetWidth()-300, ofGetHeight()-150);
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
        } else {
            vidRecorder.setup("rec"+ofGetTimestampString()+".mov", frame.getWidth(), frame.getHeight(), frameRate, sampleRate, channels);
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
