

uniform sampler2DRect fbo;

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
	//	float circleEdgeAA = 1. - (3. - (circleRadius - d ))/3.;
	float circleEdgeAA = 1. - (d + edgeAADist - circleRadius) / edgeAADist;
	
	gl_FragColor = color;
	gl_FragColor.w *= circleEdgeAA;
}
