
varying vec3 norm;
//varying vec2 uv;

varying vec3 col;
varying vec3 pos;


void main()
{
//	uv = gl_MultiTexCoord0.xy;// * texDim; <-- pass this in as a uniform
//
	norm = normalize( gl_NormalMatrix * gl_Normal );
	norm.z = abs(norm.z);
		
	col = gl_Color.xyz;
	
	pos = gl_Vertex.xyz;
	
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * (gl_Vertex * vec4(10., 1., 1., 1.));
}

