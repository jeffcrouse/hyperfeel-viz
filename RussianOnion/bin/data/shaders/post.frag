

uniform sampler2DRect fbo;

uniform vec2 center;
uniform float circleRadius;

varying vec3 col;
varying vec2 uv;

void main(void)
{
	float d = distance( center, gl_FragCoord.xy );
	if(circleRadius < d) discard;
	
	vec4 color = texture2DRect( fbo, uv );
	gl_FragColor = color;
}
