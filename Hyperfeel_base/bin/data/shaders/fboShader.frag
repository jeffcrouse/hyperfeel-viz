
uniform vec2 texDim;
uniform float radius;

uniform sampler2DRect tex;

uniform sampler2DRect deferredPass;

varying vec3 ePos;
varying vec2 uv;

void main(void)
{
//	vec2 texDim = vec2(1024.,1024.);//<-- set when these we size / resize the fbo
	vec2 screenCenter = texDim / 2.;//
	
	vec2 screenDelta = uv.xy  - screenCenter;
	
	if( distance( gl_FragCoord.xy, screenCenter ) > radius){
		discard;
	}
	
	vec4 color = texture2DRect( tex, uv );
	
	vec4 normAndDepth = texture2DRect( deferredPass, uv );
	vec3 norm = normAndDepth.xyz * 2. - 1.;
	float depth = normAndDepth.w;
	
	float fr = dot( norm, ePos ) * .5 + .5;
	float amnt = pow(fr+ .1, 4.0);
//	float alpha = pow( amnt, 2.);
	
	gl_FragColor = vec4( vec3(fr),  1. );
}