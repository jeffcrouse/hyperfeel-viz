uniform float time;

attribute vec3 tangent;
attribute vec3 binormal;

varying vec3 norm;
varying vec2 uv;


void main()
{
	uv = gl_MultiTexCoord0.xy;// * texDim; <-- pass this in as a uniform

	norm = normalize( gl_NormalMatrix * gl_Normal );
	
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
}

