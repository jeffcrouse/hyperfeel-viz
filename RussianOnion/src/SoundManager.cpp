//
//  SoundManager.cpp
//  RussianOnion
//
//  Created by Jeffrey Crouse on 8/29/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "SoundManager.h"

// -------------------------------------------------
SoundManager::~SoundManager() {
    
}

// -------------------------------------------------
void SoundManager::setup() {
    spread = 2.0;
    max_volume = 1/(float)ceil(spread);
    masterVolume = masterVolumeTarget = 0;
    audioLevelsPreview.allocate(320, 240);
    attention = attentionTarget = 0;
    meditation = meditationTarget = 0;
    
    masterMixer.setInputBusCount(2);
    
    attention_sounds.push_back("Loops/Silence.wav");
    attention_sounds.push_back("Loops/BrainWave03-Attn.wav");
    attention_sounds.push_back("Loops/BrainWave06-Attn.wav"); // DON'T USE WITH 4
    attention_sounds.push_back("Loops/Silence.wav");
    attention_sounds.push_back("Loops/BRainWave16-Attn.wav");
    attention_sounds.push_back("Loops/BrainWave07-Attn.wav");
    attention_sounds.push_back("Loops/BrainWave10-Attn.wav");
    attention_sounds.push_back("Loops/5minsilence.wav");
    attention_sounds.push_back("Loops/BrainWave09-Attn.wav");
    attention_sounds.push_back("Loops/BrainWave17-Attn.wav");
    attention_sounds.push_back("Loops/Silence.wav");
    
    meditation_sounds.push_back("Loops/Silence.wav");
    meditation_sounds.push_back("Loops/BrainWave15-Med.wav");
    meditation_sounds.push_back("Loops/BrainWave11-Med.wav");
    meditation_sounds.push_back("Loops/Silence.wav"); //BrainWave08-Med.wav"); // lower level
    meditation_sounds.push_back("Loops/BrainWave04-Both.wav");
    meditation_sounds.push_back("Loops/BrainWave12-Med.wav");
    meditation_sounds.push_back("Loops/BrainWave14-Both.wav");
    meditation_sounds.push_back("Loops/BrainWave13-Med.wav");
    meditation_sounds.push_back("Loops/Silence.wav");
    
    
    attentionVolume = new float[ attention_sounds.size() ];
    attentionLoops = new ofxAudioUnitFilePlayer[ attention_sounds.size() ];
    attentionMixer.setInputBusCount(attention_sounds.size());
    attentionMixer.connectTo(masterMixer, 0);
    for(int i=0; i<attention_sounds.size(); i++) {
        string fname = attention_sounds[i];
        attentionLoops[i].setFile(ofFilePath::getAbsolutePath(fname));
        attentionLoops[i].connectTo(attentionMixer, i);
        attentionLoops[i].loop();
        attentionMixer.setInputVolume(0, i);
        attentionVolume[i] = 0;
    }
    
    
    meditationVolume = new float[ meditation_sounds.size() ];
    meditationLoops = new ofxAudioUnitFilePlayer[ meditation_sounds.size() ];
    meditationMixer.setInputBusCount(meditation_sounds.size());
    meditationMixer.connectTo(masterMixer, 1);
    for(int i=0; i<meditation_sounds.size(); i++) {
        string fname = meditation_sounds[i];
        meditationLoops[i].setFile(ofFilePath::getAbsolutePath(fname));
        meditationLoops[i].connectTo(meditationMixer, i);
        meditationLoops[i].loop();
        meditationMixer.setInputVolume(0, i);
        meditationVolume[i] = 0;
    }
    
    masterMixer.connectTo(output);
    output.start();
    

    ofAddListener(ofEvents().update, this, &SoundManager::update );
}


// -------------------------------------------------
void SoundManager::update(ofEventArgs &args)
{
    masterVolume += (masterVolumeTarget-masterVolume) / 10.0;
    masterMixer.setOutputVolume(masterVolume);
    
    
    attention += (attentionTarget-attention) / 10.0;
    for(int i=0; i<attention_sounds.size(); i++) {
        float dist = fabs(attention-i);
        attentionVolume[i] = ofMap(dist, 0, spread, max_volume, 0, true);
        attentionMixer.setInputVolume(attentionVolume[i], i);
    }
    
    
    meditation += (meditationTarget-meditation) / 10.0;
    for(int i=0; i<meditation_sounds.size(); i++) {
        float dist = fabs(meditation-i);
        meditationVolume[i] = ofMap(dist, 0, spread, max_volume, 0, true);
        meditationMixer.setInputVolume(meditationVolume[i], i);
    }
    
    updateLevelsPreview();
}

// -------------------------------------------------
void SoundManager::startJourney(Journey* j) {
    // fade in masterMixer
    masterVolumeTarget = 1;

}

// -------------------------------------------------
void SoundManager::endJourney(Journey* j) {
    // fade out masterMixer
    masterVolumeTarget = 0;
    attentionTarget = 0;
    meditationTarget = 0;
}

// -------------------------------------------------
void SoundManager::updateJorney(Journey* j, float pct) {
    ofPoint p = j->data.getPointAtPercent(pct);
    attentionTarget = p.x * attention_sounds.size();
    meditationTarget = p.y * meditation_sounds.size();
}

// -------------------------------------------------
void SoundManager::updateLevelsPreview()
{
    float bar_width = 10;
    audioLevelsPreview.begin();
    {
        ofClear(0, 0, 0);
        ofSetColor(ofColor::wheat);
        ofRect(0, 0, ofMap(masterVolume, 0, 1, 0, audioLevelsPreview.getWidth()), bar_width);
        
        ofPushMatrix();
        {
            ofTranslate(10, audioLevelsPreview.getHeight());
            ofSetColor(ofColor::black);
            ofDrawBitmapString("ATTENTION", 0, -200);
            for(int i=0; i<attention_sounds.size(); i++) {
                ofSetColor(ofColor::wheat);
                ofRect(0, 20, bar_width, -ofMap(attentionVolume[i], 0, max_volume, 0, audioLevelsPreview.getHeight()));
                ofSetColor(ofColor::black);
                ofDrawBitmapString(ofToString(i), 0, 0);
                ofTranslate(bar_width+5, 0);
            }
            
            ofTranslate(bar_width+5, 0);
            ofSetColor(ofColor::black);
            ofDrawBitmapString("MEDITATION", 0, -200);
            for(int i=0; i<meditation_sounds.size(); i++) {
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