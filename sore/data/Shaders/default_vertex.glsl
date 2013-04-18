uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

varying vec4 color;

void main() 
{
    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
    gl_Position = projection * view * model * gl_Vertex;
    color = gl_Color;
}
