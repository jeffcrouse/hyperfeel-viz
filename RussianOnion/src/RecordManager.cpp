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
    bScreenGrabbed = false;
    
    snapshotInterval = 15;
    lastSnapshot = 0;
    
    frameDimension = ofGetHeight();
    
    ofAddListener(ofEvents().setup, this, &RecordManager::setup );
    ofAddListener(ofEvents().draw, this, &RecordManager::update );
}


// -------------------------------------------------
void RecordManager::setup(ofEventArgs &args)
{
    
    photoStrip.allocate(frameDimension*2, frameDimension*2, GL_RGB);
    photoStripSaver.allocate(frameDimension*2, frameDimension*2, OF_IMAGE_COLOR);
    
    //frame.setUseTexture(false);
    
    photoStrip.begin();
    ofClear(0);
    photoStrip.end();
    
//    vidRecorder.setVideoCodec("mpeg4");
//    vidRecorder.setVideoBitrate("2500k");
//    vidRecorder.setAudioCodec("mp3");
//    vidRecorder.setAudioBitrate("192k");
    
    //ofAddListener(httpUtils.newResponseEvent, this, &RecordManager::newResponse);
	//httpUtils.start();
    
}
/*
//--------------------------------------------------------------
void RecordManager::newResponse(ofxHttpResponse & response)
{
	cout << ofToString(response.status) + ": " + (string)response.responseBody << endl;
}*/

//--------------------------------------------------------------
void RecordManager::audioIn(float *input, int bufferSize, int nChannels)
{
    if(bJourneyInProgress && bMakeVideo && vidRecorder.isInitialized()) {
        vidRecorder.addAudioSamples(input, bufferSize, nChannels);
    }
}

// -------------------------------------------------
void RecordManager::grabScreen()
{
    if(!bScreenGrabbed)
    {
        int x = (ofGetWidth()/2) - (frameDimension/2);
        frame.grabScreen(x, 0, frameDimension, frameDimension);
        bScreenGrabbed = true;
    }
}

// -------------------------------------------------
void RecordManager::update(ofEventArgs &args)
{
    float now = ofGetElapsedTimef();

    if(bJourneyInProgress)
    {
        if(bMakeVideo && vidRecorder.isInitialized())
        {
            grabScreen();
            vidRecorder.addFrame(frame.getPixelsRef());
        }
        
        if(bMakePhotoStrips && photoStripTimes.size()>0 && now > photoStripTimes.back())
        {
            grabScreen();
            photoStrip.begin();
            switch(photoStripTimes.size())
            {
                case 1:
                    frame.draw(frameDimension, frameDimension);
                    break;
                case 2:
                    frame.draw(0, frameDimension);
                    break;
                case 3:
                    frame.draw(frameDimension, 0);
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
        grabScreen();
        stringstream path;
        path << SHARE_ROOT << "/Snapshots/" << ofGetTimestampString("%m%d%H%M%S") << ".png";
        frame.saveThreaded(path.str());
        lastSnapshot = now;
    }
    
    
    bScreenGrabbed = false;
}

// -------------------------------------------------
void RecordManager::startJourney(Journey* j, float duration)
{
    ofLogNotice() << "RecordManager::startJourney " << j->uid;
    bJourneyInProgress = true;
    
    stringstream path;
    path << SHARE_ROOT << "/" << j->uid;
    ofDirectory::createDirectory(path.str(), false, true);

    
    if(bMakePhotoStrips)
    {        
        float now = ofGetElapsedTimef();
        photoStripTimes.clear();
        photoStripTimes.push_back(now + (duration*0.9));
        photoStripTimes.push_back(now + (duration*0.6));
        photoStripTimes.push_back(now + (duration*0.4));
        photoStripTimes.push_back(now + (duration*0.1));
    }
    
    if(bMakeVideo)
    {
        stringstream settings;
        settings << " -vcodec mpeg4 "
                << " -b:v 2500k "
                << " -acodec mp3 " 
                << " -b:a 192k "
                << " -s 640x640 "
                << " -q:v 3 "
                << SHARE_ROOT << "/" << j->uid << "/video.mov";
        
        vidRecorder.setupCustomOutput(frameDimension, frameDimension, frameRate, sampleRate, channels, settings.str());
    }
}



// -------------------------------------------------
void RecordManager::endJourney(Journey* j)
{
    ofLogNotice() << "RecordManager::endJourney "<< j->uid;
    bJourneyInProgress = false;
    
    
    stringstream path;
    
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
        path << SHARE_ROOT << "/" << j->uid << "/photoStrip.png";
        
        photoStrip.readToPixels(photoStripSaver.getPixelsRef());
        photoStripSaver.saveThreaded(path.str());
        path.str("");
        
        photoStrip.begin();
        ofClear(0);
        photoStrip.end();
    }
    
    
    Json::Value json;
    json["email"] = j->email;
    json["uid"] = j->uid;
    json["created_at"] = (int)j->created_at;
    
    
    Json::StyledWriter writer;
    path << SHARE_ROOT << "/" << j->uid << "/info.json";
    ofstream fs( path.str().c_str() );
    fs << writer.write( json ) << endl;
    fs.close();
}

