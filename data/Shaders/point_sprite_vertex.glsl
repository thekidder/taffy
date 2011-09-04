uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform float halfWidth;

varying vec4 color;

void main() 
{
    float C = (gl_ProjectionMatrix[0] * halfWidth).x;
    color = gl_Color;

	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
	gl_Position = projection * view * model * gl_Vertex;

    // store size in normal.x
	gl_PointSize = gl_Normal.x / (sqrt(1.0 / (C * C)) * gl_Position.z);
}
