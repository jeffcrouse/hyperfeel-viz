//
//  RecordManager.cpp
//  RussianOnion
//
//  Created by Jeffrey Crouse on 8/29/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "RecordManager.h"

#define FRAME_WIDTH 1024
#define FRAME_HEIGHT 1024

// -------------------------------------------------
RecordManager::~RecordManager() {
    vidRecorder.close();
}

// -------------------------------------------------
RecordManager::RecordManager() {
    bRecording = false;
    sampleRate = 44100;
    channels = 2;
    frameRate = 30;
    bEnabled = true;
    
    ofAddListener(ofEvents().setup, this, &RecordManager::setup );
    ofAddListener(ofEvents().update, this, &RecordManager::update );
}


// -------------------------------------------------
void RecordManager::setup(ofEventArgs &args) {
    
    //frame.allocate(512, 512, OF_IMAGE_COLOR);

//    vidRecorder.setVideoCodec("mpeg4");
//    vidRecorder.setVideoBitrate("1500k");
//    vidRecorder.setAudioCodec("mp3");
//    vidRecorder.setAudioBitrate("192k");
    
    ofAddListener(httpUtils.newResponseEvent, this, &RecordManager::newResponse);
	httpUtils.start();
    
}

//--------------------------------------------------------------
void RecordManager::newResponse(ofxHttpResponse & response)
{
	cout << ofToString(response.status) + ": " + (string)response.responseBody << endl;
}

//--------------------------------------------------------------
void RecordManager::audioIn(float *input, int bufferSize, int nChannels){
    if(bRecording) {
        vidRecorder.addAudioSamples(input, bufferSize, nChannels);
    }
}

// -------------------------------------------------
void RecordManager::update(ofEventArgs &args) {
    if(bRecording) {
        frame.grabScreen(0, 0, FRAME_WIDTH, FRAME_HEIGHT);
        vidRecorder.addFrame(frame.getPixelsRef());
    }
}

// -------------------------------------------------
void RecordManager::startJourney(Journey* j) {
    if(!bEnabled) return;
    
    if(j->email!="")
    {
        // ofGetTimestampString("%m%d%H%M%S");
        filename = "Recordings/"+j->uid+".mov";
        ofLogNotice() << "Recording to " << filename;
        vidRecorder.setup(filename, FRAME_WIDTH, FRAME_HEIGHT, frameRate, sampleRate, channels);
        bRecording = true;
    }
    else {
        ofLogNotice() << "Not starting recording.  No email address present.";
    }
}

// -------------------------------------------------
void RecordManager::endJourney(Journey* j) {
    
    if(vidRecorder.isInitialized() && bRecording)
    {
        vidRecorder.close();
        bRecording = false;
        
        /*
         ofxHttpForm form;
         form.action = "http://cheese.local:3000/submit/video";
         form.method = OFX_HTTP_POST;
         form.addFormField("journey_id", journey_id);
         form.addFormField("email", "jeff@crouse.cc");
         form.addFile("video", filename);
         httpUtils.addForm(form);
         */
    }
}

