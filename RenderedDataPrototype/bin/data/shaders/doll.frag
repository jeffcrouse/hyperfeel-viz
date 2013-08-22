uniform float alpha;
varying vec3 lPos;
varying vec3 color;
varying vec3 ePos;
varying vec3 norm;
varying vec2 uv;

void main(void)
{
	float frScl = .3;
	float mFrScl = 1.1 - frScl;
	float lfr = pow( dot( normalize(lPos), norm ) * frScl + mFrScl, 2. );
	float unscaledFR = dot( normalize(-ePos), norm );
	float fr = unscaledFR * frScl + mFrScl;
	
	vec3 c = color;
//	if(abs(unscaledFR) < .15 || uv.y > .97){
//		c = vec3(0.);
//	}
	
	gl_FragColor = vec4( c * lfr * fr, alpha );
}
