uniform mat4 transform;

varying vec2 tex;

void main()
{
    tex = (gl_TextureMatrix[0] * gl_MultiTexCoord0).st;
    gl_Position = gl_ModelViewProjectionMatrix * transform * gl_Vertex;
}

