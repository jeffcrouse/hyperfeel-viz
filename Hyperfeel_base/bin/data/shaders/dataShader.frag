
uniform float nearClip;
uniform float farClip;

varying vec3 ePos;
varying vec3 col;
varying float vData;
varying float vDepth;



float linearizeDepth( in float d ) {
    return (2.0 * nearClip) / (farClip + nearClip - d * (farClip - nearClip));
}


void main(void)
{
	float depth = linearizeDepth( gl_FragCoord.z );
	gl_FragColor = vec4( vec3( depth), 1.);// vec4(col * pow( vData*1.5, 2.) * depth,1.);
}
