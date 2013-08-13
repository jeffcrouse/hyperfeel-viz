varying vec3 ePos;
varying vec2 uv;

void main()
{
	gl_TexCoord[0] = gl_MultiTexCoord0;
	uv = gl_MultiTexCoord0.xy;
	
	vec4 ecPosition = gl_ModelViewMatrix * gl_Vertex;
	ePos = normalize(ecPosition.xyz/ecPosition.w);
	gl_Position = gl_ProjectionMatrix * ecPosition;
}

