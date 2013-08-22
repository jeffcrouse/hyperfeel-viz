
uniform float nearClip;
uniform float farClip;

varying vec3 ePos;
varying vec3 col;
varying vec3 norm;
varying float vData;



float linearizeDepth( in float d ) {
    return (2.0 * nearClip) / (farClip + nearClip - d * (farClip - nearClip));
}


void main(void)
{
	float depth = linearizeDepth( gl_FragCoord.z );
//	gl_FragColor = vec4(col * pow( vData*1.5, 2.) * (1.-pow(depth, 2.)) * 2.,1.);
//	gl_FragColor = vec4( vec3(pow( vData*1.5, 1.) * (1.-pow(depth, 4.) )) + col*.2, 1.);
	
    gl_FragData[0] = vec4( col, 1.);
    gl_FragData[1] = vec4( normalize(norm)*.5+.5, depth );// norm, depth ) ;
}
