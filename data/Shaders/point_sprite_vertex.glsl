uniform float halfWidth;
uniform mat4 transform;

varying vec4 color;

void main() 
{
    float C = gl_ProjectionMatrix[0] * halfWidth;
    color = gl_Color;

	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
	gl_Position = gl_ModelViewProjectionMatrix * transform * gl_Vertex;

    // store size in normal.x
	gl_PointSize = gl_Normal.x / (sqrt(1.0 / (C * C)) * gl_Position.z);
}
