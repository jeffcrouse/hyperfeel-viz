//
//  HyperFeel_guiSwtich.h
//  Hyperfeel_base
//

#include "Hyperfeel_Button.h"
#include "ofxGuiGroup.h"
#include "ofParameter.h"


class ofxSwitch_ : public ofxGuiGroup {
public:
    ofxSwitch_(){
        sliderChanging = false;
    };
	
	
	ofxSwitch_ testSetup(){
		
//		add( s_1.set("s_1", "") );
//		add( s_2 );
//		add( s_3 );
		
//		switches["s_0"];
//		switches["s_1"];
//		switches["s_2"];
//		switches["s_3"];
		
//		//remove preset listeners
//		for (map<string, ofParameter<bool> >::iterator it=switches.begin(); it!=switches.end(); ++it){
//			//			it->second.removeListener( this, &testApp::switchPresets );
//			add( it->second.set( it->first, false ) );
//		}
		
	}
//    ofxSwitch_(ofParameter<VecType> value, float width = defaultWidth, float height = defaultHeight);
//	
//    ofxSwitch_ * setup(ofParameter<VecType> value, float width = defaultWidth, float height = defaultHeight);
//    ofxSwitch_ * setup(string controlName, const VecType & value, const VecType & min, const VecType & max, float width = defaultWidth, float height = defaultHeight);
//	
//    ofAbstractParameter & getParameter();
//	
//    VecType operator=(const VecType & v);
//	operator const VecType & ();
//	const VecType * operator->();
protected:
    void changeSlider(const void * parameter, float & value){cout << "chaning" << endl; };
//    void changeValue(VecType & value);
//    ofParameter<VecType> value;
    bool sliderChanging;
	
//	vector <string> switchNames;
//	map < string, ofParameter<bool> > switches;
	
	ofParameter<string> s_1;
	ofParameter<string> s_2;
	ofParameter<string> s_3;
};