#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup()
{
    ofxLibwebsockets::ClientOptions options = ofxLibwebsockets::defaultClientOptions();
    options.host = "brainz.io"; //
    options.port = 8080;
    bool connected = client.connect( options );
    
    client.addListener(this);
    ofSetFrameRate(60);
    
    //ofSetLogLevel(OF_LOG_VERBOSE);
}

//--------------------------------------------------------------
void testApp::update(){
    for(int i=0; i<journeys.size(); i++) {
        journeys[i]->update();
    }
}

//--------------------------------------------------------------
void testApp::draw(){
    for(int i=0; i<journeys.size(); i++) {
        journeys[i]->draw();
    }
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
	cout<<"got message " << endl;
    
    if ( !reader.parse( args.message, json ) ) {
        std::cout  << "Failed to parse json\n" << reader.getFormattedErrorMessages();
        return;
    }
    string route = json["route"].asString();
    
    
    if(route=="showJourney") {
		// on start up to populate the animation
        //false for not animating in
        journeys.push_back(new Journey(json["journey"], false));
    }
    else if(route=="addJourney") {
    
		//true for animating in
        journeys.push_back(new Journey(json["journey"], true));
    }
    else if(route=="removeJourney") {
        string uid = json["_id"].asString();
        for(int i=0; i<journeys.size(); i++) {
            if(journeys[i]->uid==uid) {
                journeys[i]->sayGoodbye();
            }
        }
    }
    else if(route=="tick") {
		
		//for testing, can go if iritating
        //ofLogVerbose() << "server time: " << iso8601toTimestamp(json["date"].asString());
    }
    else {
        ofLogWarning() << "Route " << route << " unknown..." << endl;
    }
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
