
varying vec3 norm;
varying vec2 uv;


void main(void)
{
//	gl_FragColor = vec4( uv, 1., 1. );
	gl_FragColor = vec4( norm*.5 + .5, 1.);
}
