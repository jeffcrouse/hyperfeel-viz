uniform vec2 texDim;
uniform sampler2DRect dataTexture;

varying vec3 norm;
varying vec2 uv;


void main(void)
{
	vec3 data = texture2DRect( dataTexture, uv * texDim ).xyz;
//	float v = uv.y; * 2. - 1.;
	float threshold = .1;
	float readingScale = .1;
	float attention = data.x * readingScale;
	float meditation = data.y * readingScale;
	
	//two smples for AA

	// if uv.y is above .
	if(  uv.y - attention  > .5 + threshold ){
		//sample the Attention
		discard;
	}
	else if( uv.y + meditation < .5-threshold ){;//meditation * v < -threshold){
		//sample the meditation
		discard;
	}
	
	
//	gl_FragColor = vec4( uv, 1., 1. );
	gl_FragColor = vec4( norm*.5 + .5 , 1.);// * vec4( data.xy, 1., 1. );
}
