
//attribute float vData;
//attribute float timeStamp;
//attribute float plusMinus;

uniform vec3 color;
uniform float radius;
uniform float offset;
uniform float curveWidth;

varying vec3 ePos;
varying vec3 col;
varying float vData;
varying float vDepth;

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
	
	pos += ( normalizedPosition * offset * data * data + normalizedPosition * curveWidth) * plusMinus;
	
	vec4 ecPosition = gl_ModelViewMatrix * vec4( pos, 1.);
	ePos = normalize(ecPosition.xyz/ecPosition.w);
	gl_Position = gl_ProjectionMatrix * ecPosition	;
	
	vDepth = gl_Position.z;
	
	col = color;
	vData = data;
}

