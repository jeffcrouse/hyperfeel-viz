//
//  User.h
//  Prototype01
//
//  Created by Jeffrey Crouse on 8/12/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "ofMain.h"

struct Reading {
    float value;
    float time;
};

class User {
public:
    static float radius_min;
    static float radius_max;
    static float multiplier_min;
    static float multiplier_max;
    static float age_max;
    
    
    ofColor color;
    vector<Reading> data;
    float startTime;
    float endTime;
    ofPoint noise[2];
    bool bIsDead;
    float radius, targetRadius;
    float alpha, targetAlpha;
    
    
    User() {
        alpha = targetAlpha = 255;
        radius = targetRadius = User::radius_max;
        
        noise[0].y = ofRandomf();
        noise[0].x = ofRandomf();
        
        noise[1].y = ofRandomf();
        noise[1].x = ofRandomf();
        
        startTime = ofGetElapsedTimef();
        endTime = startTime + 60;
        color.r = ofRandom(0, 255);
        color.g = ofRandom(0, 255);
        color.b = ofRandom(0, 255);
        bIsDead = false;
    }
    
    void update() {
        radius += (targetRadius-radius)/5.0;
        alpha += (targetAlpha-alpha)/20.0;
        noise[0].x += 0.1;
        noise[1].x += 0.2;
        
        
        float time = ofGetElapsedTimef();
        
        
        if(time < endTime){
            if(ofRandomf()>0.4)
            {
                float n = ofNoise(noise[0].x, noise[0].y) * ofNoise(noise[1].x, noise[1].y);
                Reading r;
                r.time = ofGetElapsedTimef();
                r.value = 1-n;
                data.push_back(r);
            }
        } else {
            targetAlpha = 100;
        }
    }
    
    void draw() {
        float time = ofGetElapsedTimef();
        float age = time - startTime;
        float multiplier = ofMap(radius, radius_max, radius_min, multiplier_max, multiplier_min);
        float rad = radius+(multiplier*0.5);
        float angle = ofMap(time, startTime, endTime, 0, PI*2);
        
        ofPoint current(cos(angle)*rad,sin(angle) * rad);
        
        
        
        ofPushStyle();
        ofEnableAlphaBlending();
        
        ofNoFill();
        ofSetColor(ofColor::white, 20);
        ofCircle(0, 0, rad);
        
        
        ofSetColor(color, alpha);
        if(time > endTime) {
            ofFill();
        } else {
            
            ofLine(ofPoint(0,0), current);
        }
        
        ofBeginShape();
        for(int i=0; i<data.size(); i++) {
            float angle = ofMap(data[i].time, startTime, endTime, 0, PI*2);
            float r = radius + (data[i].value * multiplier);
            float x = cos(angle) * r;
            float y = sin(angle) * r;
            ofVertex(x, y);
            //ofCurveVertex(x, y);
        }
        if(time < endTime) ofVertex(current);
        
        ofEndShape(time > endTime);
        ofDisableAlphaBlending();
        ofPopStyle();
        
    }
    
    bool dead() {
        return bIsDead;
    }
};

