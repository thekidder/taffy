#version 110

uniform sampler2D positions;
uniform sampler2D colors;
uniform sampler2D velocities;

varying vec2 tex;

void main()
{
    vec4 position = texture2D(positions, tex);
    vec4 color    = texture2D(colors, tex);
    vec4 velocity = texture2D(velocities, tex);

    gl_FragData[0] = position;
    gl_FragData[1] = color;//vec4(0.5, 0.5, 0.5, 1.0);
    gl_FragData[2] = velocity;
}
