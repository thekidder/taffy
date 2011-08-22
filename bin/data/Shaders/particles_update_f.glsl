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

    gl_FragData[0] = vec4(position.xyz + vec3(elapsed) * dataVec.xyz, position.w);
    gl_FragData[1] = vec4(color.rgb, color.a - elapsed * dataVec.w);
    gl_FragData[2] = dataVec;
}
