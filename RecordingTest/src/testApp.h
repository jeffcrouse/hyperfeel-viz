#pragma once

#include "ofMain.h"
#include "ofxAudioUnit.h"
#include "ofxUI.h"
#include "ofxVideoRecorder.h"
#include "ofxHttpUtils.h"


#define N_MEDITATION_LOOPS 6
#define N_ATTENTION_LOOPS 10


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
        void newResponse(ofxHttpResponse & response);
        void audioIn(float * input, int bufferSize, int nChannels);
        void setupUI();
  
        ofVec2f _attention;
        ofVec2f _meditation;
        float attention;
        float meditation;
        ofFbo audioLevelsPreview;
        stringstream debugMessage;
    
        //
        // Uploading
        //
        ofxHttpUtils httpUtils;
    
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
        string filename;
        string journey_id;
    
        //
        // GUI Stuff
        //
        vector <ofxUICanvas*> guis;
    
    
        //
        //  AudioUnit stuff
        //    
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
