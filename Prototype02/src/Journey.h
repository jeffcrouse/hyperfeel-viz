//
//  Journey.h
//  Prototype02
//
//  Created by Jeffrey Crouse on 8/16/13.
//  Copyright (c) 2013 __MyCompanyName__. All rights reserved.
//

#pragma once
#include "ofMain.h"
#include "Poco/DateTimeParser.h"
#include "Poco/DateTime.h"
#include "Poco/DateTimeFormat.h"
#include <json.h>

long iso8601toTimestamp(string d);

struct Reading {
    int attention;
    int meditation;
};


class Journey {
public:
    Journey(Json::Value& json, bool buildIn=true) {
        
		//corresponds tp ipod 0-6 roygbiv. 1 == orange
        client_id = ofToInt( json["client_id"].asString() );
		
		//16 digit hash
        uid = json["_id"].asString();
		
		//date of save on interface. is0 8601. 
        created_at = iso8601toTimestamp( json["created_at"].asString() );
		
		//READINGS
		//each reading is a message sent from the ipod
        for(int i=0; i<json["readings"].size(); i++) {
			//convert to timestamp
            long time = iso8601toTimestamp( json["readings"][i]["date"].asString() );
    
			//user blink data. not in every reading and when it is it is by itself
            if(json["readings"][i]["data"].isMember("blinkStrength")) {
                //
                if(json["readings"][i]["data"]["blinkStrength"].asInt() > 50)
                    blinks.push_back( time );
                
            }
			//eeg. meditation / attention
			else if(json["readings"][i]["data"].isMember("eSenseMeditation")) {
				
				//int btwn 0-100
                Reading r;
                r.meditation = json["readings"][i]["data"]["eSenseMeditation"].asInt();
                r.attention = json["readings"][i]["data"]["eSenseAttention"].asInt();
                readings.push_back( r );
            }
			//
			else {
                ofLogWarning() << "unrecognized reading...";
            }
        }
    }
    
    
    void update() {
        
    }
    
    void draw() {
        
    }
    
    int client_id;
    string uid;
    long created_at;
    vector<Reading> readings;
    vector<long> blinks;
};
