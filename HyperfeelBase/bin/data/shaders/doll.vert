varying vec3 color;
varying vec3 norm;
varying vec3 ePos;
varying vec3 lPos;

void main()
{
	norm = normalize( gl_NormalMatrix * gl_Normal );
//	norm = norm * .5 + .5;
	
	lPos = (gl_ProjectionMatrix * vec4( 0., 1000., 0., 1. ) ).xyz;
	
	vec4 ecPosition = gl_ModelViewMatrix * gl_Vertex;
	ePos = normalize(ecPosition.xyz/ecPosition.w);
	gl_Position = gl_ProjectionMatrix * ecPosition;
	
	color = gl_Color.xyz;
}

