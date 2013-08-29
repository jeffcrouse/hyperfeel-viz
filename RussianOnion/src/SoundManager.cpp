//
//  SoundManager.cpp
//  RussianOnion
//
//  Created by Jeffrey Crouse on 8/29/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#include "SoundManager.h"


// -------------------------------------------------
void SoundManager::setup() {
    
}

// -------------------------------------------------
void SoundManager::startJourney(Journey* j) {
    
}

// -------------------------------------------------
void SoundManager::endJourney(Journey* j) {
    
}

// -------------------------------------------------
void SoundManager::updateJorney(Journey* j, float pct) {
    ofPoint p = j->data.getPointAtPercent(pct);
    float attention = p.x;
    float meditation = p.y;
}