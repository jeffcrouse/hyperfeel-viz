uniform float time;
varying vec4 color;
varying vec3 ePos;
varying vec3 pos;


float noise(vec3 p) //Thx to Las^Mercury
{
	vec3 i = floor(p);
	vec4 a = dot(i, vec3(1., 57., 21.)) + vec4(0., 57., 21., 78.);
	vec3 f = cos((p-i)*acos(-1.))*(-.5)+.5;
	a = mix(sin(cos(a)*a),sin(cos(1.+a)*(1.+a)), f.x);
	a.xy = mix(a.xz, a.yw, f.y);
	return mix(a.x, a.y, f.z);
}

void main()
{
	pos = gl_Vertex.xyz;
	
	float nVal = noise( pos + vec3(100., 100., time*1. + 100. ) ) * .5 + .5;
	
	vec4 ecPosition = gl_ModelViewMatrix * gl_Vertex;
	ePos = normalize(ecPosition.xyz/ecPosition.w);
	gl_Position = gl_ProjectionMatrix * ecPosition;
	
	//texture index
	gl_PointSize = min( 10., 10. * pow( max(0., 1. - length( ecPosition.xyz )/2000. ), 2.));
	
	color = vec4( 1., 1., 1., nVal * .125 );
}



//#version 120
//uniform sampler2DRect map;
//uniform vec4 startColor;
//uniform vec4 endColor;
//uniform float rotationRate = 2.;
//uniform float nearClip;
//uniform float farClip;
//
//uniform float maxPointSize = 20.;
//uniform float minPointSize = 2;
//uniform float time = 0.;
//uniform float frameRate = .016;
//uniform float size = 3.;
//
//uniform vec3 gravity = vec3( 0., -98., 0. );
//uniform vec3 cameraPosition;
//
//varying vec3 ecPosition3;
//varying vec3 eye;
//varying vec4 color;
//varying float age;
//
//varying vec4 q;
//
//varying float tIndex;
//
//
//float PI = 3.14159265359;
//float HALF_PI = 1.57079632679;
//
//
//void main(){
//	
//	//life and death
//	float birthTime = gl_Color.r;
//	float lifeSpan = gl_Color.g;
//
//	float ageSec = (time - birthTime);
//	age = min( 1., ageSec / lifeSpan );
//	
//	//velocity
//	vec3 vel = gl_Normal.xyz;
//	float ma = sin( age * age * HALF_PI );
//	vec3 posOffset = vel * ma;
//	posOffset += gravity * ma;
//	
//	//position
//	vec4 pos = gl_Vertex + vec4(posOffset, 0.);
//	vec4  ecPosition = gl_ModelViewMatrix * pos;
//	ecPosition3 = ecPosition.xyz / ecPosition.w;
//	gl_Position = gl_ProjectionMatrix * ecPosition;
//	eye = -normalize( ecPosition3 );
//	
//	//point size
//	float attenuation = (farClip/2.)/distance(pos.xyz, cameraPosition);
//	gl_PointSize = max( minPointSize, min( maxPointSize, size * attenuation * (1. - age) ) );
//	
//	//color
//	color = mix( startColor, endColor, age );
//	
//	//rotation
//	float angle = birthTime + rotationRate*(vel.x+vel.y+vel.z);
//	q.x = 0.0f;
//	q.y = 0.0f;
//	q.z = sin(angle / 2.);
//	q.w = cos(angle / 2.);
//	
//	//texture index
//	tIndex = mod( birthTime*1., 3.);
//	
//	if( mod( birthTime*100., 1 ) == 1. ){
//		gl_PointSize += 10. * (age + age);
//	}
//	
//}
