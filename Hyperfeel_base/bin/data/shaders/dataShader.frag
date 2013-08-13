
uniform float nearClip;
uniform float farClip;

varying vec3 ePos;
varying vec3 col;
varying float vData;



float linearizeDepth( in float d ) {
    return (2.0 * nearClip) / (farClip + nearClip - d * (farClip - nearClip));
}


void main(void)
{
	float depth = linearizeDepth( gl_FragCoord.z );
	gl_FragColor = vec4(col * pow( vData*1.5, 2.),1.) * pow(1.5-depth, 1.);
}
