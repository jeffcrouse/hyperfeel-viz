varying vec2 uv;

void main()
{
	gl_TexCoord[0] = gl_MultiTexCoord0;
	uv = gl_MultiTexCoord0.xy;
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
}

