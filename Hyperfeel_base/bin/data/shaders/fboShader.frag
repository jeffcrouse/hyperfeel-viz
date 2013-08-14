
uniform vec2 texDim;
uniform float radius;
uniform float time;
uniform float minThreshold;
uniform float maxThreshold;
uniform float sampleRadius;

uniform vec3 samples[24];

uniform sampler2DRect tex;

uniform sampler2DRect deferredPass;

varying vec3 ePos;
varying vec2 uv;

float map(float value, float inputMin, float inputMax, float outputMin, float outputMax) {;
	return ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);
}

float rand(vec2 n){
	return 0.6 + 0.5 *fract(sin(dot(n.xy, vec2(12.9898, 78.233)))* 43758.5453);
}

void main(void)
{
//	vec2 texDim = vec2(1024.,1024.);//<-- set when these we size / resize the fbo
	vec2 screenCenter = texDim / 2.;//
	
	vec2 screenDelta = uv.xy  - screenCenter;
	
	if( distance( gl_FragCoord.xy, screenCenter ) > radius){
		discard;
	}
	
	vec4 color = texture2DRect( tex, uv );
	
	vec4 normAndDepth = texture2DRect( deferredPass, uv );
	vec3 norm = normAndDepth.xyz * 2. - 1.;
	float depth = normAndDepth.w;
	
	float fr = dot( norm, -ePos ) * .5 + .5;
	
	gl_FragColor = vec4( color.xyz * pow(fr+.25, 2.),  1. );
	
	float delta, mx, mn;
	float ao = 0.;
	float dscl = (1.-depth);
	dscl *= dscl;
	mx = maxThreshold * dscl;
	mn = minThreshold * dscl;
	float rad = sampleRadius * dscl;
	float rnd = 0.;
	vec3 ray;
	
	vec2 seed = gl_FragCoord.xy` + time*.01;
	for(int i=0; i<24; i+=2){
		rnd = rand( seed+float(i*i));
		
		ray = samples[i].xyz * rad * rnd;
		ray = reflect( -ray, norm);
		delta = depth - mn - ( texture2DRect( deferredPass, uv + ray.xy).w );
		ao += min( 1., ( delta > 0. ) ? delta/max( delta, mx) : (mx - delta)/mx );
	}
	
	
	gl_FragColor += vec4( (vec3(ao/12.) * .3-.15) ,1. );

	
}