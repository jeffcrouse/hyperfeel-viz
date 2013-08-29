

uniform sampler2DRect fbo;
uniform sampler2DRect mm1;
uniform sampler2DRect mm2;
uniform sampler2DRect mm3;
uniform sampler2DRect mm4;
uniform sampler2DRect mm5;

uniform vec2 center;
uniform float circleRadius;
uniform float edgeAADist;

varying vec3 col;
varying vec2 uv;

void main(void)
{
	float d = distance( center, gl_FragCoord.xy );
	if(circleRadius < d) discard;
	
	vec4 color = texture2DRect( fbo, uv );
	
	float mmScl = .5;
	vec3 glow = texture2DRect( mm1, uv * mmScl ).xyz;
	mmScl *= .5;
	glow += texture2DRect( mm2, uv * mmScl ).xyz;
	mmScl *= .5;
	glow += texture2DRect( mm3, uv * mmScl ).xyz;
	mmScl *= .5;
	glow += texture2DRect( mm4, uv * mmScl ).xyz;
	mmScl *= .5;
	glow += texture2DRect( mm5, uv * mmScl ).xyz;
	
	glow /= 5.;
	
	
	
	
	
	//	float circleEdgeAA = 1. - (3. - (circleRadius - d ))/3.;
	float circleEdgeAA = 1. - (d + edgeAADist - circleRadius) / edgeAADist;
	
	gl_FragColor = color + vec4( glow, 0.);
	gl_FragColor.w *= circleEdgeAA;
}
