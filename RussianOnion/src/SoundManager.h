//
//  SoundManager.h
//  RussianOnion
//
//  Created by Jeffrey Crouse on 8/29/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "Journey.h"
#include "ofxAudioUnit.h"

#define AMBIENT_VOLUME 0.5

class SoundManager
{
public:
    SoundManager();
    ~SoundManager();
    
    void setup(ofEventArgs &args);
    void startJourney(Journey* j);
    void endJourney(Journey* j);
    void updateJorney(Journey* j, float pct);
    void updateLevelsPreview();
    void update(ofEventArgs &args);
    void exit();
    
    bool bMuted;
    
    float spread;
    float max_volume;
    float masterVolume;
    float masterVolumeTarget;
    
    float attention, attentionTarget;
    float meditation, meditationTarget;
    
    ofFbo audioLevelsPreview;
    
    ofVec3f rotVel;
    
    vector<string> attention_sounds;
    vector<string> meditation_sounds;
    ofxAudioUnitFilePlayer* meditationLoops;
    ofxAudioUnitFilePlayer* attentionLoops;
    ofxAudioUnitFilePlayer ambientLoop;
    
    float* meditationVolume;
    float* attentionVolume;
    
    ofxAudioUnitMixer meditationMixer;
    ofxAudioUnitMixer attentionMixer;
    ofxAudioUnitMixer masterMixer;
    ofxAudioUnitOutput output;
    ofxAudioUnit lowpass;
    ofxAudioUnit bandpass;
    ofxAudioUnit varispeed;
};

