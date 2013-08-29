//
//  RecordManager.cpp
//  RussianOnion
//
//  Created by Jeffrey Crouse on 8/29/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "RecordManager.h"


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
    snapshotInterval = 15;
    lastSnapshot = 0;
    
    ofAddListener(ofEvents().setup, this, &RecordManager::setup );
    ofAddListener(ofEvents().draw, this, &RecordManager::update );
}


// -------------------------------------------------
void RecordManager::setup(ofEventArgs &args) {
    
    photoStrip.allocate(ofGetWidth()*2, ofGetHeight()*2, GL_RGB);
    exporter.allocate(ofGetWidth()*2, ofGetHeight()*2, OF_IMAGE_COLOR);
    
    frame.setUseTexture(false);
    
    photoStrip.begin();
    ofClear(0);
    photoStrip.end();
    
    vidRecorder.setVideoCodec("mpeg4");
    vidRecorder.setVideoBitrate("2500k");
    vidRecorder.setAudioCodec("mp3");
    vidRecorder.setAudioBitrate("192k");
    
    ofAddListener(httpUtils.newResponseEvent, this, &RecordManager::newResponse);
	httpUtils.start();
    
}

//--------------------------------------------------------------
void RecordManager::newResponse(ofxHttpResponse & response)
{
	cout << ofToString(response.status) + ": " + (string)response.responseBody << endl;
}

//--------------------------------------------------------------
void RecordManager::audioIn(float *input, int bufferSize, int nChannels)
{
    if(bRecording) {
        vidRecorder.addAudioSamples(input, bufferSize, nChannels);
    }
}

// -------------------------------------------------
void RecordManager::update(ofEventArgs &args)
{
    float now = ofGetElapsedTimef();
    
    if(bRecording)
    {
        frame.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
        
        if(photoStripTimes.size()>0 && now > photoStripTimes.back())
        {
            photoStrip.begin();
            switch(photoStripTimes.size())
            {
                case 1:
                    frame.draw(ofGetWidth(), ofGetHeight());
                    break;
                case 2:
                    frame.draw(0, ofGetHeight());
                    break;
                case 3:
                    frame.draw(ofGetWidth(), 0);
                    break;
                case 4:
                    frame.draw(0, 0);
                    break;
            }
            photoStrip.end();
            
            photoStripTimes.pop_back();
        }
        
        vidRecorder.addFrame(frame.getPixelsRef());
    }
    else if(now-lastSnapshot > snapshotInterval)
    {
        frame.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
        frame.saveImage("Snapshots/"+ofGetTimestampString("%m%d%H%M%S")+".png");
        lastSnapshot = now;
    }
}

// -------------------------------------------------
void RecordManager::startJourney(Journey* j, float duration)
{
    if(!bEnabled) return;
    float now = ofGetElapsedTimef();
    
    //if(j->email!="")
    {
        photoStripTimes.clear();
        photoStripTimes.push_back(now + (duration*0.9));
        photoStripTimes.push_back(now + (duration*0.6));
        photoStripTimes.push_back(now + (duration*0.4));
        photoStripTimes.push_back(now + (duration*0.1));
        photoStripFilename = "PhotoStrips/"+j->uid+".png";
        
        videoFilename = "Recordings/"+j->uid+".mov";
        vidRecorder.setup(videoFilename, ofGetWidth(), ofGetHeight(), frameRate, sampleRate, channels);
        bRecording = true;
    }
    //else ofLogNotice() << "Not starting recording.  No email address present.";
}

// -------------------------------------------------
void RecordManager::endJourney(Journey* j)
{    
    if(bRecording)
    {
        if(vidRecorder.isInitialized())
            vidRecorder.close();
        
        photoStrip.readToPixels(exporter.getPixelsRef());
        exporter.saveImage(photoStripFilename);

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

