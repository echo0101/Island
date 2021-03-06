// vertex shader for light mapping

//pass through the texture coordinates

uniform float wtime;
//uniform float wHeight;
//uniform float wTilt;

void main()
{
	float wTilt = 0.0;
	vec4 v = vec4(gl_Vertex);
	float angle = mod(wtime*0.1, 360.0) + wTilt * v.x;
	v.y = 0.5*sin((v.x/15.0)*3.0+angle);
	//angle = mod(wtime*0.1, 360.0) + wTilt * v.z;
	//v.x = wHeight*sin(v.x+angle+45.0);
	gl_Position = gl_ModelViewProjectionMatrix * v;
	//gl_Normal = sin(gl_Normal+angle);
	//pass through the texture coordinates
	gl_TexCoord[0] = gl_MultiTexCoord0;
}
