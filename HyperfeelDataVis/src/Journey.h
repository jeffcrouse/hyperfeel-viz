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

class Reading : public ofVec3f {
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
	
	float getAttention(){
		return y;
	}
	float getMeditation(){
		return z;
	}
	float getT(){
		return x;
	}
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
			long time = 0;
			//???: LB: I was getting an error saying that I couldn't convert to string. so I changed it to just convert if it is a string.
			if(json["readings"][i]["date"].isString()){
				time = iso8601toTimestamp( json["readings"][i]["date"].asString() );
			}
			
			//???: it seems that some dates are empty objects
			else if( json["readings"][i]["date"].isObject() ){
				//:???
				cout << "IT'S AN OBJECT"<< endl;
				
				vector<string> members = json["readings"][i]["date"].getMemberNames();
				for(int i=0; i<members.size(); i++){
					cout << members[i] << endl;
				}
				
				cout << endl;
				
			}
			
			
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
                r.setMeditation( json["readings"][i]["data"]["eSenseMeditation"].asFloat() / 100. );
                r.setAttention( json["readings"][i]["data"]["eSenseAttention"].asFloat() / 100. );
				
                readings.push_back( r );
            }
			//
			else {
                ofLogWarning() << "unrecognized reading...";
            }
		}
		
		
		
		if( readings.size() ){
			float step = 1. / float(readings.size() - 1);
			for(int i=0; i<readings.size(); i++){
				readings[i].setT(float(i) * step);
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
