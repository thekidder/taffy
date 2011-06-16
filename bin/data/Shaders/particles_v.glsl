//uniform sampler2D position;
uniform float halfWidth;
uniform mat4 transform;

varying vec2 tex;
varying vec4 color;

void main() 
{
	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
	//tex = (gl_TextureMatrix[0] * gl_MultiTexCoord0).st;
	//vec4 texel = vec4(texture2D(position, tex).xyz, 1.0);
	gl_Position = gl_ModelViewProjectionMatrix * transform * gl_Vertex;
    float C = gl_ProjectionMatrix[0] * halfWidth;
	gl_PointSize = gl_Normal.x / (sqrt(1.0 / (C * C)) * gl_Position.z);
    color = gl_Color;
}
