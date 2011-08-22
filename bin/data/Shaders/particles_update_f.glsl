#version 110

uniform sampler2D positions;
uniform sampler2D colors;
uniform sampler2D data;

uniform float elapsed;

varying vec2 tex;

void main()
{
    vec4 position = texture2D(positions, tex);
    vec4 color    = texture2D(colors, tex);
    vec4 dataVec  = texture2D(data, tex);

    gl_FragData[0] = position + vec4(elapsed) * dataVec;
    gl_FragData[1] = color;
    gl_FragData[2] = dataVec;
}
