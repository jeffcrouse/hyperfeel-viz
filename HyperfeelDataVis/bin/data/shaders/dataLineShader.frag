
varying vec3 ePos;
varying vec3 col;
varying vec3 pos;

varying vec3 norm;
varying vec2 uv;


void main(void)
{
	float fr = abs(dot( ePos, norm ));
//	gl_FragColor = vec4( uv, 1., 1. );

//	gl_FragColor = vec4( vec3(uv, 1.), 1.);
	
//	gl_FragColor = uv.y > .5? vec4(vec2(uv.y), 0., 1.) : vec4(vec2(abs(uv.y-1.)), 1., 1.);
	gl_FragColor = vec4( col, 1. );
}
