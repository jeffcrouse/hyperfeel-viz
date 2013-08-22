
varying vec3 ePos;
varying vec3 norm;


void main(void)
{
	
	float fr = dot( -normalize(ePos), norm ) * .5 + .5;
	
	gl_FragColor = vec4( vec3(fr), 1.);
}
