
uniform vec2 texDim;
uniform float radius;

uniform sampler2DRect tex;

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
	
	gl_FragColor = color;//vec4( uv / texDim, 1., 1. );
}