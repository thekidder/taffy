uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform float lightIntensity;

varying vec4 color;
varying vec3 position;
varying vec4 position_transform;

void main() 
{
    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
    gl_Position = projection * view * model * gl_Vertex;
    color = gl_Color;

    position = (gl_Vertex).xyz;
    position_transform = gl_Position;
}
