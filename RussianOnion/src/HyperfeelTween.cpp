 //
//  HyperfeelTween.cpp
//

#include "HyperfeelTween.h"

ofEvent <TweenEvent> TweenEvent::events;


TWEEN::TWEEN()
{
	ofAddListener(ofEvents().update, this, &TWEEN::update );
};
TWEEN::~TWEEN()
{
	ofRemoveListener(ofEvents().update, this, &TWEEN::update );
};


string TWEEN::addTween( float &value, float startVal, float endVal, float startTime, float span, string name, int tweenType, int direction )
{	
	string key = getUniqueName(name);
	tweenMap[ key ].setup( &value, startVal, endVal, startTime, span, tweenType, direction, key );
	
	return key;
}

string TWEEN::getUniqueName( string rootString )
{
	string outString = rootString;
	int index=0;
	while( tweenMap.find(outString) != tweenMap.end() ){
		outString = rootString + ofToString(index);
		index++;
	}
	
	return outString;
}


void TWEEN::update( ofEventArgs &args)
{
	elapsedTime = ofGetElapsedTimef();
	
	for( map<string, Tween>::iterator it=tweenMap.begin(); it!=tweenMap.end(); it++){
		//calc tween value
		*it->second.value = calcValue( it->second );
		
		//update any other values
		for (int j=0; j<it->second.otherValuePointers.size(); j++) {
			*it->second.otherValuePointers[j] = *it->second.value;
		}
		
		
		//start it if it isn't started
		if( !it->second.bStarted && it->second.startTime <= elapsedTime){
			it->second.bStarted = true;
			
			//on start notify
			static TweenEvent startEvent;
			startEvent.message = "started";
			startEvent.name = it->second.name;
			startEvent.value = *it->second.value;
			ofNotifyEvent( TweenEvent::events, startEvent );
		}
		
		//end it if it's ended
		else if(!it->second.bEnded && it->second.endTime <= elapsedTime){
			//
			it->second.bEnded = true;
			
			//on end notify
			static TweenEvent endEvent;
			endEvent.message = "ended";
			endEvent.name = it->second.name;
			endEvent.value = *it->second.value;
			ofNotifyEvent( TweenEvent::events, endEvent );
			
		}
		
		
		//update event
		if( it->second.bStarted && !it->second.bEnded ){
			static TweenEvent updateEvent;
			updateEvent.message = "updated";
			updateEvent.name = it->second.name;
			updateEvent.value = *it->second.value;
			ofNotifyEvent( TweenEvent::events, updateEvent );
		}
		
		//remove if it's dead
		if(it->second.bEnded && !it->second.bKeepAround){
			//remove it
			tweenMap.erase( it );
		}
	}
}

//example tween event handles
void TWEEN::tweenEventHandler( TweenEvent &e)
{
	//simple example of how to use events
	if( e.name == "aTweenName" && e.message == "started"){
		//on tween start do stuff here
	}
	if( e.name == "aTweenName" && e.message == "updated"){
		//on tween start do stuff here
	}
	if( e.name == "aTweenName" && e.message == "ended"){
		//on tween end do stuff here
	}
}


/**
 *calculations
 */
float TWEEN::calcValue(Tween &t)
{
	float k;
	if(t.value!=NULL)
	{
		
		k = ofMap(elapsedTime, t.startTime, t.endTime, 0, 1, true );
		
		switch (t.equation) {
			case TWEEN_LINEAR:
				k = k;
				break;
				
			case TWEEN_SMOOTHSTEP:
				k = calcSmoothstep ( k );
				break;
				
			case TWEEN_SMOOTHERSTEP:
				k = calcSmootherstep ( k );
				break;
				
			case TWEEN_QUADRATIC:
				k = calcQuadratic ( k, t.direction );
				break;
				
			case TWEEN_CUBIC:
				k = calcCubic ( k, t.direction );
				break;
				
			case TWEEN_QUARTIC:
				k = calcQuartic ( k, t.direction );
				break;
				
			case TWEEN_QUINTIC:
				k = calcQuintic ( k, t.direction );
				break;
				
			case TWEEN_SINUSOIDAL:
				k = calcSinusoidal ( k, t.direction );
				break;
				
			case TWEEN_EXPONENTIAL:
				k = calcExponential ( k, t.direction );
				break;
				
			case TWEEN_CIRCULAR:
				k = calcCircular ( k, t.direction );
				break;
				
			case TWEEN_ELASTIC:
				k = calcElastic ( k, t.direction );
				break;
				
			case TWEEN_BACK:
				k = calcBack ( k, t.direction );
				break;
				
			case TWEEN_BOUNCE:
				k = calcBounce ( k, t.direction );
				break;
				
				
			default:
				k = calcSmootherstep ( k );
				break;
		}
	}
	
	return ofMap( k, 0., 1., t.startVal, t.endVal, false);
}

float TWEEN::calcQuadratic( float k, int direction )
{
	
	if(direction == TWEEN_IN ){
		return k*k;
	}
	else if(direction == TWEEN_OUT ){
		return k * ( 2 - k );
	}
	else{
		if ( ( k *= 2 ) < 1 ) return 0.5 * k * k;
		return - 0.5 * ( --k * ( k - 2 ) - 1 );
	}
	
}

float TWEEN::calcCubic( float k, int direction)
{
	
	if(direction == TWEEN_IN){
		return k * k * k;
	}
	
	else if(direction == TWEEN_OUT ) {
		return --k * k * k + 1;
	}
	
	else {
		if ( ( k *= 2 ) < 1 ) return 0.5 * k * k * k;
		return 0.5 * ( ( k -= 2 ) * k * k + 2 );
	}
	
}

float TWEEN::calcQuartic( float k, int direction)
{
	
	if(direction == TWEEN_IN ) {
		return k * k * k * k;
	}
	else if( direction == TWEEN_OUT ) {
		return 1 - ( --k * k * k * k );
	}
	else{
		if ( ( k *= 2 ) < 1) return 0.5 * k * k * k * k;
		return - 0.5 * ( ( k -= 2 ) * k * k * k - 2 );
	}
	
}

float TWEEN::calcQuintic( float k, int direction)
{
	if(direction == TWEEN_IN){
		return k * k * k * k * k;
	}
	
	else if(direction == TWEEN_OUT){
		return --k * k * k * k * k + 1;
	}
	
	else {
		
		if ( ( k *= 2 ) < 1 ) return 0.5 * k * k * k * k * k;
		return 0.5 * ( ( k -= 2 ) * k * k * k * k + 2 );
		
	}
	
}

float TWEEN::calcSinusoidal( float k, int direction)
{
	if(direction == TWEEN_IN){
		return 1 - cos( k * PI / 2 );
	}
	
	else if(direction == TWEEN_OUT){
		return sin( k * PI / 2 );
	}
	
	else {
		
		return 0.5 * ( 1 - cos( PI * k ) );
		
	}
	
}

float TWEEN::calcExponential( float k, int direction)
{
	if(direction == TWEEN_IN){
		return k == 0 ? 0 : pow( 1024.f, k - 1.f );
	}
	
	else if(direction == TWEEN_OUT){
		return k == 1 ? 1 : 1 - pow( 2.f, - 10.f * k );
	}
	
	else {
		
		if ( k == 0 ) return 0;
		if ( k == 1 ) return 1;
		if ( ( k *= 2 ) < 1 ) return 0.5 * pow( 1024.f, k - 1.f );
		return 0.5 * ( -pow( 2.f, - 10.f * ( k - 1.f ) ) + 2.f );
		
	}
	
}

float TWEEN::calcCircular( float k, int direction)
{
	if(direction == TWEEN_IN){
		return 1 - sqrt( 1.f - k * k );
	}
	
	else if(direction == TWEEN_OUT){
		return sqrt( 1.f - ( --k * k ) );
	}
	
	else {
		
		if ( ( k *= 2 ) < 1) return - 0.5 * ( sqrt( 1.f - k * k) - 1);
		return 0.5 * ( sqrt( 1.f - ( k -= 2) * k) + 1);
		
	}
	
}

float TWEEN::calcElastic(float k, int direction)
{
	if( direction == TWEEN_IN ) {
		float s, a = 0.1, p = 0.4;
		if ( k == 0 ) return 0;
		if ( k == 1 ) return 1;
		if ( !a || a < 1 ) { a = 1; s = p / 4; }
		else s = p * asin( 1 / a ) / ( 2 * PI );
		return - ( a * pow( 2, 10 * ( k -= 1 ) ) * sin( ( k - s ) * ( 2 * PI ) / p ) );
		
	}
	
	else if(direction == TWEEN_OUT){
		
		float s, a = 0.1, p = 0.4;
		if ( k == 0 ) return 0;
		if ( k == 1 ) return 1;
		if ( !a || a < 1 ) { a = 1; s = p / 4; }
		else s = p * asin( 1 / a ) / ( 2 * PI );
		return ( a * pow( 2, - 10 * k) * sin( ( k - s ) * ( 2 * PI ) / p ) + 1 );
		
	}
	
	else {
		
		float s, a = 0.1, p = 0.4;
		if ( k == 0 ) return 0;
		if ( k == 1 ) return 1;
		if ( !a || a < 1 ) { a = 1; s = p / 4; }
		else s = p * asin( 1 / a ) / ( 2 * PI );
		if ( ( k *= 2 ) < 1 ) return - 0.5 * ( a * pow( 2, 10 * ( k -= 1 ) ) * sin( ( k - s ) * ( 2 * PI ) / p ) );
		return a * pow( 2, -10 * ( k -= 1 ) ) * sin( ( k - s ) * ( 2 * PI ) / p ) * 0.5 + 1;
		
	}
	
}

float TWEEN::calcBack(float k, int direction)
{
	if( direction == TWEEN_IN ) {
		float s = 1.70158;
		return k * k * ( ( s + 1 ) * k - s );
		
	}
	
	else if(direction == TWEEN_OUT){
		
		float s = 1.70158;
		return --k * k * ( ( s + 1 ) * k + s ) + 1;
		
	}
	
	else {
		
		float s = 1.70158 * 1.525;
		if ( ( k *= 2 ) < 1 ) return 0.5 * ( k * k * ( ( s + 1 ) * k - s ) );
		return 0.5 * ( ( k -= 2 ) * k * ( ( s + 1 ) * k + s ) + 2 );
		
	}
	
}

float TWEEN::calcBounce(float k, int direction)
{
	if( direction == TWEEN_IN ) {
		return calcBounceIn( k );
	}
	
	else if(direction == TWEEN_OUT){
		
		return calcBounceOut(k);
		
	}
	
	else {
		return calcBounceInOut(k);
	}
	
}

float TWEEN::calcBounceIn( float k )
{
	return 1 -calcBounceOut( 1 - k );
}

float TWEEN::calcBounceOut( float k )
{
	if ( k < ( 1 / 2.75 ) ) {
		
		return 7.5625 * k * k;
		
	} else if ( k < ( 2 / 2.75 ) ) {
		
		return 7.5625 * ( k -= ( 1.5 / 2.75 ) ) * k + 0.75;
		
	} else if ( k < ( 2.5 / 2.75 ) ) {
		
		return 7.5625 * ( k -= ( 2.25 / 2.75 ) ) * k + 0.9375;
		
	} else {
		
		return 7.5625 * ( k -= ( 2.625 / 2.75 ) ) * k + 0.984375;
		
	}
}

float TWEEN::calcBounceInOut( float k )
{
	if ( k < 0.5 ) return calcBounceIn( k * 2 ) * 0.5;
	return calcBounceOut( k * 2 - 1 ) * 0.5 + 0.5;
}

float TWEEN::calcSmoothstep( float x)
{
	return x*x*(3 - 2*x);
}

float TWEEN::calcSmootherstep( float x )
{
	return x*x*x*(x*(x*6 - 15) + 10);
}
