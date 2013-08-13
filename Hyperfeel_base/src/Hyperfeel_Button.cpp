#include "Hyperfeel_Button.h"

Hyperfeel_Button::Hyperfeel_Button(){
	value.setSerializable(false);
}

Hyperfeel_Button* Hyperfeel_Button::setup(string toggleName, float width, float height){
	setName(toggleName);
	b.x = 0;
	b.y = 0;
	b.width = width;
	b.height = height;
	bGuiActive = false;
	value = false;
	checkboxRect.set(1, 1, b.height - 2, b.height - 2);

	ofRegisterMouseEvents(this,OF_EVENT_ORDER_BEFORE_APP);

	value.addListener(this,&Hyperfeel_Button::valueChanged);

	return this;
}

bool Hyperfeel_Button::mouseReleased(ofMouseEventArgs & args){
	bool wasGuiActive = bGuiActive;
	bGuiActive = false;
	if(wasGuiActive && b.inside(ofPoint(args.x,args.y))){
		return true;
	}else{
		return false;
	}
}

bool Hyperfeel_Button::mouseMoved(ofMouseEventArgs & args){
	return ofxToggle::mouseMoved(args);
}

bool Hyperfeel_Button::mousePressed(ofMouseEventArgs & args){
	return ofxToggle::mousePressed(args);
}

bool Hyperfeel_Button::mouseDragged(ofMouseEventArgs & args){
	return ofxToggle::mouseDragged(args);
}

void Hyperfeel_Button::valueChanged(bool & v){
	ofNotifyEvent(triggerEvent);
}
