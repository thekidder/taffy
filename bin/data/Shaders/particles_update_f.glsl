#version 110

uniform sampler2D positions;
uniform sampler2D colors;
uniform sampler2D data;
uniform float elapsed;

uniform sampler2D positions_spawn;
uniform sampler2D colors_spawn;
uniform sampler2D data_spawn;

varying vec2 tex;

void main()
{
    vec4 position = texture2D(positions, tex);
    vec4 color    = texture2D(colors, tex);
    vec4 dataVec  = texture2D(data, tex);

    if(color.a > 0.0)
    {
        gl_FragData[0] = vec4(position.xyz + vec3(elapsed) * dataVec.xyz, position.w);
        gl_FragData[1] = vec4(color.rgb * vec3(1.0 - dataVec.w * 0.1), color.a - elapsed * dataVec.w);
        gl_FragData[2] = dataVec;
    }
    else
    {
        gl_FragData[0] = texture2D(positions_spawn, tex);
        gl_FragData[1] = texture2D(colors_spawn, tex);
        gl_FragData[2] = texture2D(data_spawn, tex);
    }
}
