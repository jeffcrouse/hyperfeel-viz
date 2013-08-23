uniform float facingRatio;
uniform float readingThreshold;
uniform float readingScale;
uniform float alpha;
uniform float dataSmoothing;

uniform vec2 texDim;
uniform sampler2DRect dataTexture;

varying vec3 color;
varying vec3 ePos;
varying vec3 norm;
varying vec2 uv;

float AACoefficient = .00125;

float map(float value, float inputMin, float inputMax, float outputMin, float outputMax) {;
	return ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);
}

void main(void)
{
	vec2 data = texture2DRect( dataTexture, uv * texDim ).xy;
	
	//smooth it out a bit
	vec2 dataLeft = texture2DRect( dataTexture, uv * texDim+vec2(1., 0.)).xy;
	vec2 dataRight = texture2DRect( dataTexture, uv * texDim-vec2(1., 0.) ).xy;
	
	data += (dataLeft*.5 + dataRight*.5) * dataSmoothing;
	data *= 1. - dataSmoothing;
	
	float attention = uv.y - data.x * readingScale;
	float meditation = uv.y + data.y * readingScale;
	
	// if uv.y is above .
	float upThreshold = .5 + readingThreshold;
	float downThreshold = .5-readingThreshold;
	float a = alpha;
	if(  attention  > upThreshold ){
		//sample the Attention
		discard;
	}
	
	else if( meditation < downThreshold ){
		//sample the meditation
		discard;
	}
	
	//gradient alpha
	else if( attention > upThreshold - AACoefficient ){
		a = map( attention, upThreshold-AACoefficient, upThreshold, alpha , 0.);
	}
	else if( meditation < downThreshold + AACoefficient ){
		a = map( meditation, downThreshold+AACoefficient, downThreshold, alpha , 0.);
	}

	//facing ratio
	float fr = dot(-ePos, norm) * facingRatio + 1. - facingRatio;
	
	//color
	gl_FragColor = vec4( color * fr, a);// * vec4( data.xy, 1., 1. );
}

