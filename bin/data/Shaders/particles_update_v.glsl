uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

varying vec2 tex;

void main()
{
    tex = (gl_TextureMatrix[0] * gl_MultiTexCoord0).st;
    gl_Position = projection * view * model * gl_Vertex;
}

