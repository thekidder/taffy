uniform mat4 transform;

varying vec4 color;

void main() 
{
    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
    gl_Position = gl_ModelViewProjectionMatrix * transform * gl_Vertex;
    color = gl_Color;
}
