
//attribute float vData;
//attribute float timeStamp;
//attribute float plusMinus;

uniform vec3 color;
uniform float time;
uniform float radius;
uniform float offset;
uniform float curveWidth;

varying vec3 ePos;
varying vec3 col;
varying float vData;
varying vec3 norm;

float TWO_PI = 6.28318530718;
float PI = 3.14159265359;
float HALF_PI = 1.57079632679;

void main()
{
	float data = gl_Vertex.x;
	float timeStamp = gl_Vertex.y;
	float plusMinus = gl_Vertex.z;
	

//	float radius = 100.;
//	float offset = 50.;
	vec3 normalizedPosition = vec3( sin(timeStamp * TWO_PI), cos(timeStamp * TWO_PI), 0. );
	vec3 pos = normalizedPosition * radius;
	
	float timeOffset = sin(timeStamp * TWO_PI + time) * cos(timeStamp * TWO_PI + time) *.5 + .5;
	pos += ( normalizedPosition * offset * data * data * timeOffset + normalizedPosition * curveWidth) * plusMinus;
	
	vec4 ecPosition = gl_ModelViewMatrix * vec4( pos, 1.);
	ePos = normalize(ecPosition.xyz/ecPosition.w);
	gl_Position = gl_ProjectionMatrix * ecPosition	;
	
	vec3 nrm = vec3(normalizedPosition.xy * plusMinus, 1.5);
	norm = normalize( gl_NormalMatrix * nrm );

	col = color;
	vData = data;
}

