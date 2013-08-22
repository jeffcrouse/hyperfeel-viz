varying vec3 color;
varying vec3 ePos;
varying vec3 norm;
varying vec2 uv;


void main()
{
	color = gl_Color.xyz;
	uv = gl_MultiTexCoord0.xy;// * texDim; <-- pass this in as a uniform

	norm = normalize( gl_NormalMatrix * gl_Normal );
	
//	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
	
	vec4 ecPosition = gl_ModelViewMatrix * gl_Vertex;
	ePos = normalize(ecPosition.xyz/ecPosition.w);
	gl_Position = gl_ProjectionMatrix * ecPosition;
}

