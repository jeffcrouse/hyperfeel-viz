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

class Reading : public ofVec4f {
public:
	//LB: changed this to float so that we could build vbos directly from the reading vector
	void setAttention(float attention){
		y = attention;
	};
	void setMeditation(float meditation){
		z = meditation;
	};
	void setT(float t ){
		x = t;
	}
	
	void setTime(float t ){
		w = t;
	}
	
	float getAttention(){
		return y;
	}
	float getMeditation(){
		return z;
	}
	float getT(){
		return x;
	}
	float getTime(){
		return w;
	}
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
        // JRC NOTE: No longer corresponds to this -- it's the name of the device that sent it...
        client_id = json["client_id"].asInt();
		
		//16 digit hash
        uid = json["_id"].asString();
		
		//date of save on interface. is0 8601.
        created_at = iso8601toTimestamp( json["created_at"].asString() );
		
		//READINGS
		//each reading is a message sent from the ipod
        for(int i=0; i<json["readings"].size(); i++) {
			float meditation = json["readings"][i]["data"]["meditation"].asFloat() * .01;
            float attention = json["readings"][i]["data"]["attention"].asFloat() * .01;
            Reading r;
            r.setTime( iso8601toTimestamp( json["readings"][i]["date"].asString() ) ); //convert to timestamp
            r.setMeditation( meditation ); //int btwn 0-100
            r.setAttention( attention ) ; //int btwn 0-100
            data.addVertex( ofVec2f(attention, meditation) );
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
        
        long start = readings.front().getTime();
        long end = readings.back().getTime();
        long duration = end-start;
        ofLogNotice() << "Constructed Journey " << uid << " with " << readings.size() << " readings duration: " << duration;
		
		//t is a value btwn 0-1 proportionate to it's index position
		if( readings.size() ){
			float step = 1. / float(readings.size() - 1);
			for(int i=0; i<readings.size(); i++){
				readings[i].setT(float(i) * step);
			}
		}
	}
	
	// Animate out and delete
    void sayGoodbye() {
        
    }
	
	void update() {
		
	}
	
	void draw() {
	}
	
    ofPolyline data;
	int client_id;
	string uid;
	long created_at;
	vector<Reading> readings;
	vector<Event> events;
    vector<long> blinks;
};
