uniform sampler2D position;
uniform float pointSize;
uniform mat4 view;

varying vec2 tex;

void main() 
{
	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
	tex = (gl_TextureMatrix[0] * gl_MultiTexCoord0).st;
	vec4 texel = vec4(texture2D(position, tex).xyz, 1.0);
	gl_Position = gl_ModelViewProjectionMatrix * view * texel;
	gl_PointSize = pointSize;
}
