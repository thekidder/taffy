uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform sampler2D positions;

void main() 
{
	vec2 tex = (gl_TextureMatrix[0] * gl_MultiTexCoord0).st;
    vec4 position = vec4(texture2D(positions, tex).xyz, 1.0);

	gl_Position = projection * view * model * position;
	gl_PointSize = 1.0;
}
