uniform vec3 blendColor;
uniform float noiseExponent;
uniform float noiseSpread;
uniform float noiseMixExponent;
uniform float noisePosScale;
uniform float sampleVal;
uniform float animateIn;
uniform float time;
uniform float facingRatio;
uniform float readingThreshold;
uniform float readingScale;
uniform float alpha;
uniform float dataSmoothing;

uniform float slope;

uniform vec2 texDim;
uniform sampler2DRect dataTexture;
float AACoefficient = .00125;


uniform float frExpo;
uniform float deltaExpo;
uniform float noiseSurfaceSampleScale;

varying vec4 ecPosition;
varying vec3 color;
varying vec3 eye;
varying vec3 norm;
varying vec2 uv;
varying float delta;

uniform float nearClip;
uniform float farClip;

float noise(vec3 p) //Thx to Las^Mercury
{
	vec3 i = floor(p);
	vec4 a = dot(i, vec3(1., 57., 21.)) + vec4(0., 57., 21., 78.);
	vec3 f = cos((p-i)*acos(-1.))*(-.5)+.5;
	a = mix(sin(cos(a)*a),sin(cos(1.+a)*(1.+a)), f.x);
	a.xy = mix(a.xz, a.yw, f.y);
	return mix(a.x, a.y, f.z);
}

float rand(vec2 n){
	//	return 0.6 + 0.5 *fract(sin(dot(n.xy, vec2(12.9898, 78.233)))* 43758.5453);
	return 0.6 + .5 * fract( sin( dot( n.xy, n.yx * 10.)));
}
vec3 mod289(vec3 x) {
	return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec2 mod289(vec2 x) {
	return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec3 permute(vec3 x) {
	return mod289(((x*34.0)+1.0)*x);
}

float snoise(vec2 v)
{
	const vec4 C = vec4(0.211324865405187,  // (3.0-sqrt(3.0))/6.0
						0.366025403784439,  // 0.5*(sqrt(3.0)-1.0)
						-0.577350269189626,  // -1.0 + 2.0 * C.x
						0.024390243902439); // 1.0 / 41.0
											// First corner
	vec2 i  = floor(v + dot(v, C.yy) );
	vec2 x0 = v -   i + dot(i, C.xx);
	
	// Other corners
	vec2 i1;
	//i1.x = step( x0.y, x0.x ); // x0.x > x0.y ? 1.0 : 0.0
	//i1.y = 1.0 - i1.x;
	i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
	// x0 = x0 - 0.0 + 0.0 * C.xx ;
	// x1 = x0 - i1 + 1.0 * C.xx ;
	// x2 = x0 - 1.0 + 2.0 * C.xx ;
	vec4 x12 = x0.xyxy + C.xxzz;
	x12.xy -= i1;
	
	// Permutations
	i = mod289(i); // Avoid truncation effects in permutation
	vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
					 + i.x + vec3(0.0, i1.x, 1.0 ));
	
	vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
	m = m*m ;
	m = m*m ;
	
	// Gradients: 41 points uniformly over a line, mapped onto a diamond.
	// The ring size 17*17 = 289 is close to a multiple of 41 (41*7 = 287)
	
	vec3 x = 2.0 * fract(p * C.www) - 1.0;
	vec3 h = abs(x) - 0.5;
	vec3 ox = floor(x + 0.5);
	vec3 a0 = x - ox;
	
	// Normalise gradients implicitly by scaling m
	// Approximation of: m *= inversesqrt( a0*a0 + h*h );
	m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );
	
	// Compute final noise value at P
	vec3 g;
	g.x  = a0.x  * x0.x  + h.x  * x0.y;
	g.yz = a0.yz * x12.xz + h.yz * x12.yw;
	return 130.0 * dot(m, g);
}

float map(float value, float inputMin, float inputMax, float outputMin, float outputMax) {;
	return ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);
}

void main(void)
{
	
	vec2 data = texture2DRect( dataTexture, uv * texDim ).xy;
	
	//smooth it out a bit
	vec2 dataLeft = texture2DRect( dataTexture, uv * texDim+vec2(1., 0.)).xy;
	vec2 dataRight = texture2DRect( dataTexture, uv * texDim-vec2(1., 0.) ).xy;
	
	float animateInScale = pow( animateIn, 4.);
	
	//smooth and scale the data
	data = dataSmoothing * (dataLeft*.5 + dataRight*.5) + (1. - dataSmoothing) * data;
	data *= readingScale;// * animateInScale;
	
	float attention = data.x;
	float meditation = data.y;
	
	float threshold = readingThreshold;
	
	//if we're ahead of animate in we won't show up
	if( uv.x - slope > animateIn){
		discard;
	}
	
	//smooth the end
	float diffBack = min(1., max(0., (animateIn - uv.x+slope) / slope ));
	if(animateIn < slope)	diffBack *= animateIn / slope;
	
	
	//smooth the oher end
	float animVal = min(1., max(0., map( animateIn, 1. - slope, 1., 0., 1. ) ) );
	float diffFront = min(1., max( 0.,  map( uv.x, 0., slope, animVal, 1.) ) );
	
	//smooth the front and back exponetially. goes from linear to expo curves
	diffFront = 1. - pow( 1. - diffFront, 3.0);
	diffBack = 1. - pow( 1. - diffBack, 3.0);
	
	
	//if above .5 we're in the top half handling attention
	if( uv.y > .5){
		//move our threshold up into the top half
		threshold = .5 + threshold * diffBack * diffFront;
		
		if( uv.y - attention * diffBack * diffFront> threshold ){
			discard;
		}
		
	}
	//otherwise we're in the bottom half and handling meditation
	else{
		//invert the threshold (measuring from the top down)
		threshold = .5 - threshold * diffBack * diffFront;
		
		if(uv.y + meditation * diffBack * diffFront < threshold){
			discard;
		}
		
	}
	
	float a = alpha;
	
	//	//gradient alpha
	//	else if( attention > upThreshold - AACoefficient ){
	//		a = map( attention, upThreshold-AACoefficient, upThreshold, alpha , 0.);
	//	}
	//	else if( meditation < downThreshold + AACoefficient ){
	//		a = map( meditation, downThreshold+AACoefficient, downThreshold, alpha , 0.);
	//	}
	
	
	//facing ratio
	float fr = dot( eye, norm) * facingRatio + 1. - facingRatio;
	
	//color
	vec3 col = blendColor * fr;// color * fr;

	//bars
	float barSample = uv.x*628. + time; // 942
	a *= pow( abs(sin( barSample ) + cos( barSample )), 4.);
	
	if(a <= 0.)	discard;
	
	//noise
	
//	float nVal = pow( abs( noise( vec3( vec2(uv.x + sampleVal, uv.y + sampleVal + time*.005), 10.)  * noisePosScale )*2. - 1. ), 2.);
	
	float nVal = max( 0., snoise( vec2(uv.x + sampleVal, uv.y + sampleVal + time*.005)  * noisePosScale ) );
	
	nVal = pow( nVal, noiseExponent );
	
	a *=  mix( pow( nVal, 1.+sampleVal*1.), 1., pow( sampleVal, noiseMixExponent ) + noiseSpread);
	
	
	gl_FragColor = vec4( col, a);
}
