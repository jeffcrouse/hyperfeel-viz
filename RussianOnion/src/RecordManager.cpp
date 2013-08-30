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
    
    sampleRate = 44100;
    channels = 2;
    frameRate = 30;

    bJourneyInProgress = false;
    bMakeVideo = true;
    bMakeSnapshots = true;
    bMakePhotoStrips = true;
    
    snapshotInterval = 15;
    lastSnapshot = 0;
    
    ofAddListener(ofEvents().setup, this, &RecordManager::setup );
    ofAddListener(ofEvents().draw, this, &RecordManager::update );
}


// -------------------------------------------------
void RecordManager::setup(ofEventArgs &args)
{
    
    photoStrip.allocate(ofGetWidth()*2, ofGetHeight()*2, GL_RGB);
    exporter.allocate(ofGetWidth()*2, ofGetHeight()*2, OF_IMAGE_COLOR);
    
    //frame.setUseTexture(false);
    
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
    if(bJourneyInProgress && bMakeVideo && vidRecorder.isInitialized()) {
        vidRecorder.addAudioSamples(input, bufferSize, nChannels);
    }
}


// -------------------------------------------------
void RecordManager::update(ofEventArgs &args)
{
    float now = ofGetElapsedTimef();
   
    if(bJourneyInProgress)
    {
        bool bScreenGrabbed = false;
        if(bMakeVideo && vidRecorder.isInitialized())
        {
            frame.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
            bScreenGrabbed = true;
            vidRecorder.addFrame(frame.getPixelsRef());
        }
        
        if(bMakePhotoStrips && photoStripTimes.size()>0 && now > photoStripTimes.back())
        {
            if(!bScreenGrabbed) {
                frame.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
                bScreenGrabbed = true;
            }
            
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
        
    }
    else if(bMakeSnapshots && now-lastSnapshot > snapshotInterval)
    {
        frame.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
        frame.saveImage("Snapshots/"+ofGetTimestampString("%m%d%H%M%S")+".png");
        lastSnapshot = now;
    }    
}

// -------------------------------------------------
void RecordManager::startJourney(Journey* j, float duration)
{
    bJourneyInProgress = true;
    
//    if(j->email=="") {
//        ofLogNotice() << "Not starting recording.  No email address present.";
//        return;
//    }
    
    if(bMakePhotoStrips)
    {
        photoStripFilename = "PhotoStrips/"+j->uid+".png";
        float now = ofGetElapsedTimef();
        photoStripTimes.clear();
        photoStripTimes.push_back(now + (duration*0.9));
        photoStripTimes.push_back(now + (duration*0.6));
        photoStripTimes.push_back(now + (duration*0.4));
        photoStripTimes.push_back(now + (duration*0.1));
    }
    
    if(bMakeVideo)
    {
        videoFilename = "Recordings/"+j->uid+".mov";
        vidRecorder.setup(videoFilename, ofGetWidth(), ofGetHeight(), frameRate, sampleRate, channels);
    }
}

// -------------------------------------------------
void RecordManager::endJourney(Journey* j)
{    
    bJourneyInProgress = false;
    
    if(bMakeVideo && vidRecorder.isInitialized())
    {
        vidRecorder.close();
    }
    
    /*
     ofxHttpForm form;
     form.action = "http://cheese.local:3000/submit/video";
     form.method = OFX_HTTP_POST;
     form.addFormField("journey_id", journey_id);
     form.addFormField("email", "jeff@crouse.cc");
     form.addFile("video", filename);
     httpUtils.addForm(form);
     */

    
    if(bMakePhotoStrips)
    {
        photoStrip.readToPixels(exporter.getPixelsRef());
        exporter.saveImage(photoStripFilename);
    }
}

