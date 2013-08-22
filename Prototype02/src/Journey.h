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
    long time;
};

class Event {
public:
    long time;
    string type;
    Json::Value data;
};

class Journey {
public:
    Journey(Json::Value& json, bool buildIn=true) {
        
		//corresponds tp ipod 0-6 roygbiv. 1 == orange
        client_id = json["client_id"].asInt();
		
		//16 digit hash
        uid = json["_id"].asString();
		
		//date of save on interface. is0 8601. 
        created_at = iso8601toTimestamp( json["created_at"].asString() );
		
		//READINGS
		//each reading is a message sent from the ipod
        for(int i=0; i<json["readings"].size(); i++) {
			
            Reading r;
            r.time = iso8601toTimestamp( json["readings"][i]["date"].asString() ); //convert to timestamp
            r.meditation = json["readings"][i]["data"]["meditation"].asInt(); //int btwn 0-100
            r.attention = json["readings"][i]["data"]["attention"].asInt(); //int btwn 0-100
            readings.push_back( r );
        }
        
        //EVENTS
        for(int i=0; i<json["events"].size(); i++) {
            
            Event e;
            e.time = iso8601toTimestamp( json["events"][i]["date"].asString() ); //convert to timestamp
            e.type = json["events"][i]["eventType"].asString();
            e.data = json["events"][i]["data"];
            events.push_back( e );
        }
        
        long start = readings.front().time;
        long end = readings.back().time;
        long duration = end-start;
        ofLogNotice() << "Constructed Journey " << uid << " with " << readings.size() << " readings duration: " << duration;
        
    }
    
    // Animate out and delete
    void sayGoodbye() {
        
    }
    
    void update() {
        
    }
    
    void draw() {
 
    }
    
    int client_id;
    string uid;
    long created_at;
    vector<Reading> readings;
    vector<Event> events;
    vector<long> blinks;
};
