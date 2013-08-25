#pragma once

#include "ofMain.h"
#include "ofxAudioUnit.h"
#include "ofxUI.h"
#include "ofxVideoRecorder.h"

#define N_MEDITATION_LOOPS 6
#define N_ATTENTION_LOOPS 10


/**
 *  NOTICE:
 *  Make sure that you have installed SoundFlower https://code.google.com/p/soundflower/
 *  And set System Preferences > Sound > Output to "Soundflower (2ch)"
 *  This will direct all audio from the entire system to a virtual 
 *  You'll also have to launch Soundflowerbed and select "Built-in Output" 
**/

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
        void exit();
    
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void guiEvent(ofxUIEventArgs &e);
        void audioIn(float * input, int bufferSize, int nChannels);
        void setupUI();
  
        ofVec2f _attention;
        ofVec2f _meditation;
        float attention;
        float meditation;
        ofFbo audioLevelsPreview;
        stringstream debugMessage;
    
        //
        // Recording stuff
        //
        bool bRecording;
        ofxVideoRecorder    vidRecorder;
        ofSoundStream       soundStream;
        ofImage             frame;
        int sampleRate;
        int channels;
        int frameRate;
    
        //
        // GUI Stuff
        //
        vector <ofxUICanvas*> guis;
    
    
        //
        //  AudioUnit stuff
        //
//        float _reverb;
//        float _distortion;
//        float _compression;
//        float _delay;
//        float _lowpass;
//        float _varispeed;
    
        float spread;
        ofxAudioUnitOutput output;
        //ofxAudioUnitTap tap;
        ofxAudioUnitFilePlayer meditationLoops[N_MEDITATION_LOOPS];
        ofxAudioUnitFilePlayer attentionLoops[N_ATTENTION_LOOPS];
        float meditationVolume[N_MEDITATION_LOOPS];
        float attentionVolume[N_ATTENTION_LOOPS];
    
        ofxAudioUnitMixer meditationMixer;
        ofxAudioUnitMixer attentionMixer;
        ofxAudioUnitMixer masterMixer;
    
        ofxAudioUnit reverb;
        ofxAudioUnit varispeed;
        ofxAudioUnit lowpass;
        //ofxAudioUnit compressor;
        ofxAudioUnit delay;
        ofxAudioUnit distortion;
    
    
        //
        // Testing video content
        //
        ofVideoGrabber cam;
};
