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
    
}

// -------------------------------------------------
RecordManager::RecordManager() {
    ofAddListener(ofEvents().setup, this, &RecordManager::setup );
    ofAddListener(ofEvents().update, this, &RecordManager::update );
}


// -------------------------------------------------
void RecordManager::setup(ofEventArgs &args) {

}

// -------------------------------------------------
void RecordManager::update(ofEventArgs &args) {
    
}

// -------------------------------------------------
void RecordManager::startJourney(Journey* j) {
    
}

// -------------------------------------------------
void RecordManager::endJourney(Journey* j) {
    
}