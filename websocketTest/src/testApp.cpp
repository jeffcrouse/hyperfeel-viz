#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofxLibwebsockets::ClientOptions options = ofxLibwebsockets::defaultClientOptions();
    options.host = "je44.com";
    options.port = 8080;
    //options.protocol = "of-protocol";
    bool connected = client.connect( options );
    
    client.addListener(this);
    ofSetFrameRate(60);
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){

}


//--------------------------------------------------------------
void testApp::onConnect( ofxLibwebsockets::Event& args ){
    cout<<"on connected"<<endl;
}

//--------------------------------------------------------------
void testApp::onOpen( ofxLibwebsockets::Event& args ){
    cout<<"on open"<<endl;
}

//--------------------------------------------------------------
void testApp::onClose( ofxLibwebsockets::Event& args ){
    cout<<"on close"<<endl;
}

//--------------------------------------------------------------
void testApp::onIdle( ofxLibwebsockets::Event& args ){
    cout<<"on idle"<<endl;
}

//--------------------------------------------------------------
void testApp::onMessage( ofxLibwebsockets::Event& args ){
    cout<<"got message "<<args.message<<endl;
    
    Json::Value json;   // will contains the root value after parsing.
    Json::Reader reader;
    bool parsingSuccessful = reader.parse( args.message, json );
    if ( !parsingSuccessful ) {
        std::cout  << "Failed to parse configuration\n" << reader.getFormattedErrorMessages();
        return;
    }
    
    cout << "ROUTE: " << json["route"].asString() << endl;
}

//--------------------------------------------------------------
void testApp::onBroadcast( ofxLibwebsockets::Event& args ){
    cout<<"got broadcast "<<args.message<<endl;
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

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
