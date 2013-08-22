uniform float frExpo;
uniform float time;
uniform float spread;
uniform float twistAngle;
uniform int twist;
uniform float radius;

varying vec4 ecPosition;
varying vec3 ePos;
varying vec3 norm;
varying vec2 uv;

varying vec3 col;
varying vec3 pos;


float TWO_PI = 6.28318530718;
float PI = 3.14159265359;
float HALF_PI = 1.57079632679;

vec3 rotateVectorByQuaternion( vec3 v, vec4 q ) {
	
	vec3 dest = vec3( 0.0 );
	
	float x = v.x, y  = v.y, z  = v.z;
	float qx = q.x, qy = q.y, qz = q.z, qw = q.w;
	
	// calculate quaternion * vector
	float ix =  qw * x + qy * z - qz * y,
	iy =  qw * y + qz * x - qx * z,
	iz =  qw * z + qx * y - qy * x,
	iw = -qx * x - qy * y - qz * z;
	
	// calculate result * inverse quaternion
	dest.x = ix * qw + iw * -qx + iy * -qz - iz * -qy;
	dest.y = iy * qw + iw * -qy + iz * -qx - ix * -qz;
	dest.z = iz * qw + iw * -qz + ix * -qy - iy * -qx;
	
	return dest;
}

void main()
{
//	bool twist = true;
//	float radius = 10.;
	
	float t = gl_Vertex.x;
	col = gl_Color.xyz;
	pos = gl_Vertex.xyz;
	norm = normalize( gl_NormalMatrix * gl_Normal );
	uv = gl_MultiTexCoord0.xy;// * texDim; <-- pass this in as a uniform
	
	float amp = abs(uv.y*2. - 1.);// * spread + 1.;
	
	float bumpOffset = t*TWO_PI + time;
	float mSpread = 1. - spread;
	vec3 wpos = vec3( 0., pos.yz );// * amp);
	
	wpos.y = pos.y * (1. + pow(spread*2., 2.)) + sin(bumpOffset)*spread + mSpread, cos(bumpOffset)*spread + mSpread * .4;
	wpos.z = pos.z * (.1 + pow(spread*2., .6)) + sin(bumpOffset)*spread + mSpread, cos(bumpOffset)*spread + mSpread * .4;
	
	vec3 dir = vec3(0., radius ,0.);
	
	
	vec4 q, q1;

	//rotation
	
	
	//twist
	float angle;
	if(twist == 1){
		angle = twistAngle ;//+ t*TWO_PI + time;// t * TWO_PI * 7. + time;
		q1.x = 0.;
		q1.y = sin(angle / 2.);
		q1.z = cos(angle / 2.);
		q1.w = 0.;
		wpos = rotateVectorByQuaternion(wpos, q1);
		rotateVectorByQuaternion( norm, q1);
	}
	angle= t * TWO_PI;
	q.x = 0.;
	q.y = 0.;
	q.z = sin(angle / 2.);
	q.w = cos(angle / 2.);
	wpos = rotateVectorByQuaternion( wpos, q) + rotateVectorByQuaternion(dir, q);
	pos = wpos;
	norm = normalize( rotateVectorByQuaternion( norm, q) );
	
	norm.z = abs(norm.z);//double sided so let's flip the z

	ecPosition = gl_ModelViewMatrix * vec4(wpos, 1.);
	ePos = normalize(ecPosition.xyz/ecPosition.w);
	gl_Position = gl_ProjectionMatrix * ecPosition;
}

