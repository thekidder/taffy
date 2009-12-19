varying vec4 color;

void main() 
{
	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
	gl_Position = ftransform();
	color = gl_Color;
}
