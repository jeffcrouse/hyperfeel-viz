//
//  RecordManager.h
//  RussianOnion
//
//  Created by Jeffrey Crouse on 8/29/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "Journey.h"
#include "ofxVideoRecorder.h"
//#include "ofxHttpUtils.h"
#include "ofxThreadedImageSaver.h"


#define LOCAL_SHARE_ROOT "/Users/jeff/Desktop/Sharing"
#define REMOTE_SHARE_ROOT "/Users/jeff/Desktop/Remote_Share"


class RecordManager
{
public:
    RecordManager();
    ~RecordManager();
    
    void setup(ofEventArgs &args);
    void update(ofEventArgs &args);
    void startJourney(Journey* j, float duration);
    void endJourney(Journey* j);    
    //void newResponse(ofxHttpResponse & response);
    void audioIn(float * input, int bufferSize, int nChannels);
    void grabScreen();
    
    //
    // Uploading
    //
    //ofxHttpUtils httpUtils;
    
    //
    // Recording stuff
    //
    bool bMakeVideo;
    bool bMakeSnapshots;
    bool bMakePhotoStrips;
    bool bJourneyInProgress;
    ofxVideoRecorder vidRecorder;
    
    int frameDimension;
    bool bScreenGrabbed;
    
    vector<float> photoStripTimes;
    ofFbo photoStrip;
    ofxThreadedImageSaver photoStripSaver;
    
    ofxThreadedImageSaver frame;
    float lastSnapshot, snapshotInterval;
    int sampleRate;
    int channels;
    int frameRate;
};

