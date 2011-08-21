uniform mat4 transform;
uniform sampler2D positions;

void main() 
{
	vec2 tex = (gl_TextureMatrix[0] * gl_MultiTexCoord0).st;
    vec4 position = vec4(texture2D(positions, tex).xyz, 1.0);

	gl_Position = gl_ModelViewProjectionMatrix * transform * position;
	gl_PointSize = 1.0;
}
