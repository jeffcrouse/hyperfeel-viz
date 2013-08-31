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
#include "ofxHttpUtils.h"

class RecordManager
{
public:
    RecordManager();
    ~RecordManager();
    
    void setup(ofEventArgs &args);
    void update(ofEventArgs &args);
    void startJourney(Journey* j, float duration);
    void endJourney(Journey* j);    
    void newResponse(ofxHttpResponse & response);
    void audioIn(float * input, int bufferSize, int nChannels);
    
    //
    // Uploading
    //
    ofxHttpUtils httpUtils;
    
    //
    // Recording stuff
    //
    bool bMakeVideo;
    bool bMakeSnapshots;
    bool bMakePhotoStrips;
    bool bJourneyInProgress;
    ofxVideoRecorder vidRecorder;
    
    vector<float> photoStripTimes;
    string photoStripFilename;
    ofFbo photoStrip;
    ofImage exporter;
    
    ofImage frame;
    float lastSnapshot, snapshotInterval;
    int sampleRate;
    int channels;
    int frameRate;
    string videoFilename;
    string journey_id;
};
