//
//  SoundManager.h
//  RussianOnion
//
//  Created by Jeffrey Crouse on 8/29/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "Journey.h"

class SoundManager
{
public:
    void setup();
    void startJourney(Journey* j);
    void endJourney(Journey* j);
    void updateJorney(Journey* j, float pct);
};

