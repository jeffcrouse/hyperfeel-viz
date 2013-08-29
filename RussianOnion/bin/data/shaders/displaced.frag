uniform float animateIn;
uniform float time;
uniform float facingRatio;
uniform float readingThreshold;
uniform float readingScale;
uniform float alpha;
uniform float dataSmoothing;

uniform float slope;

uniform vec2 texDim;
uniform sampler2DRect dataTexture;
float AACoefficient = .00125;


uniform float frExpo;
uniform float deltaExpo;
uniform float noiseSurfaceSampleScale;

varying vec4 ecPosition;
varying vec3 color;
varying vec3 eye;
varying vec3 norm;
varying vec2 uv;
varying float delta;

uniform float nearClip;
uniform float farClip;

float rand(vec2 n){
//	return 0.6 + 0.5 *fract(sin(dot(n.xy, vec2(12.9898, 78.233)))* 43758.5453);
	return 0.6 + .5 * fract( sin( dot( n.xy, n.yx * 10.)));
}

vec4 permute( vec4 x ) {
	return mod( ( ( x * 34.0 ) + 1.0 ) * x, 289.0 );
}
vec4 taylorInvSqrt( vec4 r ) {
	return 1.79284291400159 - 0.85373472095314 * r;
}
float snoise( vec3 v ) {
	const vec2 C = vec2( 1.0 / 6.0, 1.0 / 3.0 );
	const vec4 D = vec4( 0.0, 0.5, 1.0, 2.0 );
	// First corner
	vec3 i  = floor( v + dot( v, C.yyy ) );
	vec3 x0 = v - i + dot( i, C.xxx );
	// Other corners
	vec3 g = step( x0.yzx, x0.xyz );
	vec3 l = 1.0 - g;
	vec3 i1 = min( g.xyz, l.zxy );
	vec3 i2 = max( g.xyz, l.zxy );
	vec3 x1 = x0 - i1 + 1.0 * C.xxx;
	vec3 x2 = x0 - i2 + 2.0 * C.xxx;
	vec3 x3 = x0 - 1. + 3.0 * C.xxx;
	// Permutations
	i = mod( i, 289.0 );
	vec4 p = permute( permute( permute(
									   i.z + vec4( 0.0, i1.z, i2.z, 1.0 ) )
							  + i.y + vec4( 0.0, i1.y, i2.y, 1.0 ) )
					 + i.x + vec4( 0.0, i1.x, i2.x, 1.0 ) );
	// Gradients
	// ( N*N points uniformly over a square, mapped onto an octahedron.)
	float n_ = 1.0 / 7.0; // N=7
	vec3 ns = n_ * D.wyz - D.xzx;
	vec4 j = p - 49.0 * floor( p * ns.z *ns.z );  //  mod(p,N*N)
	vec4 x_ = floor( j * ns.z );
	vec4 y_ = floor( j - 7.0 * x_ );    // mod(j,N)
	vec4 x = x_ *ns.x + ns.yyyy;
	vec4 y = y_ *ns.x + ns.yyyy;
	vec4 h = 1.0 - abs( x ) - abs( y );
	vec4 b0 = vec4( x.xy, y.xy );
	vec4 b1 = vec4( x.zw, y.zw );
	
	vec4 s0 = floor( b0 ) * 2.0 + 1.0;
	vec4 s1 = floor( b1 ) * 2.0 + 1.0;
	vec4 sh = -step( h, vec4( 0.0 ) );
	vec4 a0 = b0.xzyw + s0.xzyw * sh.xxyy;
	vec4 a1 = b1.xzyw + s1.xzyw * sh.zzww;
	vec3 p0 = vec3( a0.xy, h.x );
	vec3 p1 = vec3( a0.zw, h.y );
	vec3 p2 = vec3( a1.xy, h.z );
	vec3 p3 = vec3( a1.zw, h.w );
	
	// Normalise gradients
	vec4 norm = taylorInvSqrt( vec4( dot( p0, p0 ), dot( p1, p1 ), dot( p2, p2 ), dot( p3, p3 ) ) );
	p0 *= norm.x;
	p1 *= norm.y;
	p2 *= norm.z;
	p3 *= norm.w;
	
	// Mix final noise value
	vec4 m = max( 0.6 - vec4( dot( x0, x0 ), dot( x1, x1 ), dot( x2, x2 ), dot( x3, x3 ) ), 0.0 );
	m = m * m;
	return 42.0 * dot( m*m, vec4( dot( p0, x0 ), dot( p1, x1 ),
								 dot( p2, x2 ), dot( p3, x3 ) ) );
}

float linearizeDepth( in float d ) {
    return (2.0 * nearClip) / (farClip + nearClip - d * (farClip - nearClip));
}

float map(float value, float inputMin, float inputMax, float outputMin, float outputMax) {;
	return ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);
}

void main(void)
{
	
	vec2 data = texture2DRect( dataTexture, uv * texDim ).xy;
	
	//smooth it out a bit
	vec2 dataLeft = texture2DRect( dataTexture, uv * texDim+vec2(1., 0.)).xy;
	vec2 dataRight = texture2DRect( dataTexture, uv * texDim-vec2(1., 0.) ).xy;
	
	float animateInScale = pow( animateIn, 4.);
	
	//smooth and scale the data
	data = dataSmoothing * (dataLeft*.5 + dataRight*.5) + (1. - dataSmoothing) * data;
	data *= readingScale;// * animateInScale;
	
	float attention = data.x;
	float meditation = data.y;
	
	float threshold = readingThreshold;
	
	//if we're ahead of animate in we won't show up
	if( uv.x - slope > animateIn){
		discard;
	}
	
	//smooth the end
	float diffBack = min(1., max(0., (animateIn - uv.x+slope) / slope ));
	if(animateIn < slope)	diffBack *= animateIn / slope;
	
	
	//smooth the oher end
	float animVal = min(1., max(0., map( animateIn, 1. - slope, 1., 0., 1. ) ) );
	float diffFront = min(1., max( 0.,  map( uv.x, 0., slope, animVal, 1.) ) );
	
	//smooth the front and back exponetially. goes from linear to expo curves
	diffFront = 1. - pow( 1. - diffFront, 3.0);
	diffBack = 1. - pow( 1. - diffBack, 3.0);
	
	
	//if above .5 we're in the top half handling attention
	if( uv.y > .5){
		//move our threshold up into the top half
		threshold = .5 + threshold * diffBack * diffFront;
		
		if( uv.y - attention * diffBack * diffFront> threshold ){
			discard;
		}

	}
	//otherwise we're in the bottom half and handling meditation
	else{
		//invert the threshold (measuring from the top down)
		threshold = .5 - threshold * diffBack * diffFront;
		
		if(uv.y + meditation * diffBack * diffFront < threshold){
			discard;
		}
		
	}

	float a = alpha;

//	//gradient alpha
//	else if( attention > upThreshold - AACoefficient ){
//		a = map( attention, upThreshold-AACoefficient, upThreshold, alpha , 0.);
//	}
//	else if( meditation < downThreshold + AACoefficient ){
//		a = map( meditation, downThreshold+AACoefficient, downThreshold, alpha , 0.);
//	}
	
//	a *= min(1., animateIn * 2. );
	

	//facing ratio
	float fr = dot( eye, norm) * facingRatio + 1. - facingRatio;
	
	//color
	vec3 col = color * fr;
	
	gl_FragColor = vec4( col, a); // * vec4(uv, 1., 1.)
}
