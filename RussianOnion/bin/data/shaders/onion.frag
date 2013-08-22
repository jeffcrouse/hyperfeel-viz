uniform float readingThreshold;
uniform float readingScale;
uniform float alpha;

uniform vec2 texDim;
uniform sampler2DRect dataTexture;

varying vec3 color;
varying vec3 ePos;
varying vec3 norm;
varying vec2 uv;


void main(void)
{
	vec3 data = texture2DRect( dataTexture, uv * texDim ).xyz;
	float attention = data.x * readingScale;
	float meditation = data.y * readingScale;
	
	//two smples for AA

	// if uv.y is above .
	if(  uv.y - attention  > .5 + readingThreshold ){
		//sample the Attention
		discard;
	}
	else if( uv.y + meditation < .5-readingThreshold ){
		//sample the meditation
		discard;
	}
	
	float fr = (dot(-ePos, norm)) * .5 + .5;
	
//	gl_FragColor = vec4( uv, 1., 1. );
	gl_FragColor = vec4( color * fr, alpha);// * vec4( data.xy, 1., 1. );
}

