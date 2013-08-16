
varying vec3 lPos;
varying vec3 color;
varying vec3 ePos;
varying vec3 norm;

void main(void)
{
	float frScl = .3;
	float mFrScl = 1.1 - frScl;
	float lfr = pow( dot( normalize(lPos), norm ) * frScl + mFrScl, 2. );
	float fr = dot( normalize(-ePos), norm ) * frScl + mFrScl;
	gl_FragColor = vec4( color * lfr * fr, 1. );
}
