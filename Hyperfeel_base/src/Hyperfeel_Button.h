#pragma once

#include "ofxToggle.h"
#include "ofParameter.h"

class Hyperfeel_Button : public ofxToggle{
	friend class ofPanel;
	
public:
	Hyperfeel_Button();
	Hyperfeel_Button* setup(string toggleName, float width = defaultWidth, float height = defaultHeight);

	virtual bool mouseReleased(ofMouseEventArgs & args);
	virtual bool mouseMoved(ofMouseEventArgs & args);
	virtual bool mousePressed(ofMouseEventArgs & args);
	virtual bool mouseDragged(ofMouseEventArgs & args);

	template<class ListenerClass, typename ListenerMethod>
	void addListener(ListenerClass * listener, ListenerMethod method){
		ofAddListener(triggerEvent,listener,method);
	}

	template<class ListenerClass, typename ListenerMethod>
	void removeListener(ListenerClass * listener, ListenerMethod method){
		ofRemoveListener(triggerEvent,listener,method);
	}
	
	bool getValue(){
		return value;
	}
	
	void setValue( bool v ){
		value = v;
	}

private:
	ofEvent<void> triggerEvent;
	void valueChanged(bool & v);

};
