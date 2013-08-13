#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofSetFrameRate(60);
    ofSetCircleResolution(100);
    
    User::radius_min = ofGetHeight()/10.0;
    User::radius_max = (ofGetHeight()/2.0) - User::radius_min;
}

//--------------------------------------------------------------
void testApp::update(){
    
    for(int i=0; i<users.size(); i++) {
        users[i]->update();
        if(users[i]->dead()) {
            delete users[i];
            users.erase(users.begin()+i);
        }
    }
}

//--------------------------------------------------------------
void testApp::draw(){
    ofBackground(0);
    ofSetColor(255);
    ofDrawBitmapString(ofToString(ofGetFrameRate()), 10, 20);
    
    
    ofPushMatrix();
    ofTranslate(ofGetWidth()/2.0, ofGetHeight()/2.0);
    ofRotateZ(-90);
    for(int i=users.size()-1; i>-1; i--) {
        users[i]->draw();
    }
    
    ofPopMatrix();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
    if(key=='u') {
        for(int i=0; i<users.size(); i++) {
            users[i]->targetRadius -= 40;
        }
        User* u = new User();
        users.push_back( u );
    }
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
