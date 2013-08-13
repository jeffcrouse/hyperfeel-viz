
uniform vec2 texDim;
uniform float radius;

uniform sampler2DRect tex;

uniform sampler2DRect deferredPass;

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
	vec3 norm = normAndDepth.xyz*2. - 1.;
	float depth = normAndDepth.w;
	
	gl_FragColor = vec4( color.xyz * depth, 1. );
}