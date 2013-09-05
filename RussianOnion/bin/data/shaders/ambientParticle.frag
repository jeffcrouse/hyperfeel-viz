uniform float time;
varying vec3 ePos;
varying vec3 pos;

varying vec4 color;

//vec3 rotateVectorByQuaternion( vec3 v, vec4 q ) {
//	
//	vec3 dest = vec3( 0.0 );
//	
//	float x = v.x, y  = v.y, z  = v.z;
//	float qx = q.x, qy = q.y, qz = q.z, qw = q.w;
//	
//	// calculate quaternion * vector
//	float ix =  qw * x + qy * z - qz * y,
//	iy =  qw * y + qz * x - qx * z,
//	iz =  qw * z + qx * y - qy * x,
//	iw = -qx * x - qy * y - qz * z;
//	
//	// calculate result * inverse quaternion
//	dest.x = ix * qw + iw * -qx + iy * -qz - iz * -qy;
//	dest.y = iy * qw + iw * -qy + iz * -qx - ix * -qz;
//	dest.z = iz * qw + iw * -qz + ix * -qy - iy * -qx;
//	
//	return dest;
//}
//
//float linearizeDepth( in float d ) {
//    return (2.0 * nearClip) / (farClip + nearClip - d * (farClip - nearClip));
//}

void main()
{
	gl_FragColor = color;
	
}