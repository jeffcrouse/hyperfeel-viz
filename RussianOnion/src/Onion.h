//
//  onion.h
//  RussianOnion
//
//  Created by laserstorm on 8/22/13.
//
//

#include "ofMain.h"
#include "Journey.h";

class Onion{
public:
	Onion(){
		bIsSetup = false;
	};
	~Onion(){
		//???: do we need to do more to deallocate this
		dataTexture.clear();
		
		randomNumer = ofRandom(-1, 1);
	};
	
	void setup( Journey* j ){
		
		//set up data texture
		textureDim.set( j->readings.size(), 1 );
		int totalPixels = j->readings.size()*3;
		vector< float > data( totalPixels );
		for(int i=0; i<j->readings.size(); i++){
			data[i*3] = ofClamp( j->readings[i].getAttention(), 0, 1);
			data[i*3+1] = ofClamp( j->readings[i].getMeditation(), 0, 1);
			data[i*3+2] = ofClamp( j->readings[i].getT(), 0, 1);
		}
		
		dataTexture.loadData( &data[0], j->readings.size(), 1, GL_RGB );
		
		bIsSetup = true;
		
		
		randomNumer = ofRandom(-100, 100);
	}
	
	ofTexture dataTexture;
	ofVec2f textureDim;
	ofNode transform;
	ofColor color;
	bool bIsSetup;
	float sampleVal;
	float randomNumer;
};