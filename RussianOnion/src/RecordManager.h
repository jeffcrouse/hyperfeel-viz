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
    void startJourney(Journey* j);
    void endJourney(Journey* j);
};

