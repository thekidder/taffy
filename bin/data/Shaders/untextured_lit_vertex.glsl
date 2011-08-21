uniform mat4 transform;
uniform vec3 lightPos;
uniform float lightIntensity;

varying vec4 color;
varying vec3 lightTransformed;
varying vec3 position;

void main() 
{
    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
    gl_Position = gl_ModelViewProjectionMatrix * transform * gl_Vertex;
    color = gl_Color;

    lightTransformed = (transform * vec4(lightPos, 1.0)).xyz;
    position = (transform * gl_Vertex).xyz;
}
