attribute float attention;
attribute float meditation;
attribute float timeStamp;
attribute float plusMinus;

uniform float radius;
uniform float offset;

varying vec3 ePos;
varying vec3 color;

float TWO_PI = 6.28318530718;
float PI = 3.14159265359;
float HALF_PI = 1.57079632679;

void main()
{
//	float radius = 100.;
//	float offset = 50.;
	vec3 normalizedPosition = vec3( sin(timeStamp * TWO_PI), cos(timeStamp * TWO_PI), 0. );
	vec3 pos = normalizedPosition * radius;
	
	pos += normalizedPosition * offset * attention * plusMinus;
	
	vec4 ecPosition = gl_ModelViewMatrix * vec4( pos, 1.);
	ePos = normalize(ecPosition.xyz/ecPosition.w);
	gl_Position = gl_ProjectionMatrix * ecPosition	;
	
	color = vec3( attention, meditation, timeStamp );
	
//	gl_Position = vec4( normalizedPosition * .5, 1.);
}

