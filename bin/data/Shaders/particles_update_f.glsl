#version 110

uniform sampler2D positions;
uniform sampler2D colors;
uniform sampler2D data;
uniform float elapsed;
uniform float t;

uniform sampler2D positions_spawn;
uniform sampler2D colors_spawn;
uniform sampler2D data_spawn;

varying vec2 tex;

float pos_rand(vec2 co)
{
    return fract(sin(dot(co, vec2(12.9898,78.233))) * 43758.5453);
}

/* 
   gl_FragData:
   positions
   colors
   data
*/

void main()
{
    vec4 position = texture2D(positions, tex);
    vec4 color    = texture2D(colors, tex);
    vec4 dataVec  = texture2D(data, tex);

    if(color.a > 0.02)
    {
        vec3 new_color = color.rgb - vec3(0.0, elapsed * 0.05, elapsed * dataVec.w);
        new_color = max(new_color, vec3(0.0, 0.0, 0.6));

        vec3 vel = dataVec.xyz;
        vel.y += 1.0 * smoothstep(-2.9, -3.0, position.y);

        gl_FragData[0] = vec4(position.xyz + vec3(elapsed) * vel, position.w - 0.2 * dataVec.w * elapsed);
        gl_FragData[1] = vec4(new_color.rgb, color.a - elapsed * dataVec.w);
        gl_FragData[2] = vec4(vel, dataVec.w);
    }
    else
    {
        gl_FragData[0] = texture2D(positions_spawn, tex);
        gl_FragData[1] = texture2D(colors_spawn, tex);
        gl_FragData[2] = texture2D(data_spawn, tex);
    }
}
