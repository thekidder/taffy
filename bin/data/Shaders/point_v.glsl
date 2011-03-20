uniform float pointSize;
uniform mat4 view;

void main() 
{
	gl_Position = gl_ModelViewProjectionMatrix * view * gl_Vertex;
	gl_PointSize = pointSize;
}
