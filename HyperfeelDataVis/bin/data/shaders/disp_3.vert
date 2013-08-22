uniform float displacement;
uniform float time;
uniform float roundingWeight;

attribute vec3 tangent;
attribute vec3 binormal;

varying vec4 ecPosition;

varying vec4 p;
varying vec3 eye;
varying vec3 norm;
varying vec2 uv;
varying float delta;

float offset = displacement;
float noiseScale = .0015;

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

vec3 normalFrom3Points( vec3 p0, vec3 p1, vec3 p2){
	return normalize( cross( p2-p1, p0-p1 ) );
}

float remapValue( float t ){
	//	P(t) = P0*t^2 + P1*2*t*(1-t) + P2*(1-t)^2
	float p0 = -.2;
	float p1 = roundingWeight;
	float p2 = 1.;
	
	return pow( p0*t, 2.) + p1*2.*t*(1.-t) + pow(p2*(1.-t), 2.);
}

float scldNoise( vec3 s ){
	
	
	float outval = snoise( s ) * (snoise( s * 2.) * .25 + .75);
//	outval = 1. - pow( 1. - outval, 2. );
	return remapValue( outval*.5 + .5);
}
vec4 getOffset( vec3 s, vec3 n ){
	return vec4( n * offset * scldNoise( s ), 0.);
}


void main()
{

	float timeScaled = time * -.3;
	float tangentScale = 10.;
	
	uv = gl_MultiTexCoord0.xy;
	
	norm = gl_NormalMatrix *  gl_Normal;
	vec3 vTangent = gl_NormalMatrix * (-tangent * tangentScale);
	vec3 bitangent = gl_NormalMatrix * (binormal * tangentScale);
	//	vec3 vTangent = -normalize( cross( binormal, norm ) ) * tangentScale;
	
	vec3 animationOffset = vec3(100.,33.,timeScaled);
	vec3 no = norm * offset;
	
	ecPosition = gl_ModelViewMatrix * gl_Vertex;
	
	vec3 samplePos = ecPosition.xyz * noiseScale + animationOffset;
	vec3 samplePos1 = (ecPosition.xyz + vTangent) * noiseScale + animationOffset;
	vec3 samplePos2 = (ecPosition.xyz + bitangent) * noiseScale + animationOffset;
	
	delta = scldNoise( samplePos );
	vec3 deformedPos = no * delta;
	vec3 deformedTangent = vTangent + no * scldNoise( samplePos1 );
	vec3 deformedBitangent = bitangent + no * scldNoise( samplePos2 );
	
	norm = normalize( normalize( normalFrom3Points( deformedTangent, deformedPos, deformedBitangent ).xyz ));
	ecPosition += vec4( deformedPos, 0.);
	eye = -normalize(ecPosition.xyz);
	
	gl_Position = gl_ProjectionMatrix * ecPosition;
	
	p = gl_Position;
	
}