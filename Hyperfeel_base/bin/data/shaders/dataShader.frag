
varying vec3 ePos;
varying vec3 col;
varying float vData;

void main(void)
{
	gl_FragColor = vec4(col * pow( vData*1.5, 2.),1.);
}
