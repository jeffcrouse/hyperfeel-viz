//
//  HyperfeelTween.h
//


#include "ofMain.h"

enum TweenEquation {
	TWEEN_LINEAR = 0,
	TWEEN_SMOOTHSTEP = 1,
	TWEEN_SMOOTHERSTEP = 2,
	TWEEN_QUADRATIC = 3,
	TWEEN_CUBIC = 4,
	TWEEN_QUARTIC = 5,
	TWEEN_QUINTIC = 6,
	TWEEN_SINUSOIDAL = 7,
	TWEEN_EXPONENTIAL = 8,
	TWEEN_CIRCULAR = 9,
	TWEEN_ELASTIC = 10,
	TWEEN_BACK = 11,
	TWEEN_BOUNCE = 12
};

enum TweenDirection {
	TWEEN_IN=0,
	TWEEN_OUT=1,
	TWEEN_INOUT=2,
};

class Tween{
public:
	Tween(){}
	Tween( float *_value, float _startVal, float _endVal, float _startTime=ofGetElapsedTimef(), float _span=1, int _equation=TWEEN_SMOOTHSTEP, int _direction=TWEEN_IN, string _name = "tween_"  ){
		
		setup(_value, _startVal, _endVal, _startTime, _span, _equation, _direction, _name );
	};
	~Tween(){
	};
	
	void setup(float *_value, float _startVal, float _endVal, float _startTime, float _span, int _equation, int _direction, string _name  ){
		startTime = _startTime;
		span = _span;
		endTime = startTime + span;
		startVal = _startVal;
		endVal = _endVal;
		
		bStarted = false;
		bEnded = false;
		value = ( _value != NULL )? _value : &val;
		
		equation = _equation;
		direction = _direction;
		
		name = _name;
		bKeepAround = false;
	};
	
	string name;
	float *value;
	float val;
	float startTime, span, endTime, t;
	float startVal, endVal;
	bool bStarted, bEnded;
	
	int equation, direction;
	
	vector<bool*> startedPointers;
	vector<bool*> endedPointers;
	vector<float*> otherValuePointers;
	
	bool bKeepAround;
};

class TweenEvent : public ofEventArgs {
    
public:
	
	//	Tween * tween;
	string message;
	string name;
	float value;
    
    TweenEvent() {
    }
	static ofEvent <TweenEvent> events;
};

class TWEEN{
public:
	TWEEN();
	~TWEEN();
	
	
	string addTween( float &value, float startVal, float endVal, float startTime, float span, string name, int tweenType=TWEEN_LINEAR, int direction=TWEEN_IN );
	string getUniqueName( string rootString );
	
	void update( ofEventArgs &args);
	
	void tweenEventHandler( TweenEvent &e);
	
	Tween* getTween( string key ){
		return &tweenMap[ key ];
	}
	
	//calculations
	float calcValue(Tween &t);
	//equation methods
	float calcQuadratic( float k, int direction );
	float calcCubic( float k, int direction);
	float calcQuartic( float k, int direction) ;
	float calcQuintic( float k, int direction) ;
	float calcSinusoidal( float k, int direction) ;
	float calcExponential( float k, int direction) ;
	float calcCircular( float k, int direction) ;
	float calcElastic(float k, int direction) ;
	float calcBack(float k, int direction) ;
	float calcBounce(float k, int direction) ;
	float calcBounceIn( float k );
	float calcBounceOut( float k );
	float calcBounceInOut( float k );
	float calcSmoothstep( float x);
	float calcSmootherstep( float x);
	
	map<string, Tween> tweenMap;
	float elapsedTime;
};