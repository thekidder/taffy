uniform float pointSize;
uniform mat4 transform;

void main() 
{
	gl_Position = gl_ModelViewProjectionMatrix * transform * gl_Vertex;
	gl_PointSize = pointSize;
}
