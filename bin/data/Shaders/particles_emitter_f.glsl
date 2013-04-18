#version 110

uniform float seed;
uniform float t;
uniform float old_t;
uniform vec3 pos; 

uniform float energy;
uniform float beat;

varying vec2 tex;

float pos_rand(vec2 co)
{
    return fract(sin(dot(co, vec2(12.9898,78.233))) * 43758.5453);
}

float rand(vec2 co)
{
    return 2.0 * pos_rand(co) - 1.0;
}

vec3 rand_spherical(vec2 co, float radius)
{
    vec3 unit_sphere = vec3(
        rand(tex * co),
        rand(tex * co * 374.13),
        rand(tex * co * 98713.634));

    unit_sphere *= radius / 
        sqrt(unit_sphere.x * unit_sphere.x + unit_sphere.y * unit_sphere.y + unit_sphere.z * unit_sphere.z);

    return unit_sphere;
}

vec4 mod289(vec4 x)
{
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 permute(vec4 x)
{
  return mod289(((x*34.0)+1.0)*x);
}

vec4 taylorInvSqrt(vec4 r)
{
  return 1.79284291400159 - 0.85373472095314 * r;
}

vec3 fade(vec3 t) {
  return t*t*t*(t*(t*6.0-15.0)+10.0);
}

// Classic Perlin noise
float cnoise(vec3 P)
{
  vec3 Pi0 = floor(P); // Integer part for indexing
  vec3 Pi1 = Pi0 + vec3(1.0); // Integer part + 1
  Pi0 = vec3(mod289(Pi0.xyzx));
  Pi1 = vec3(mod289(Pi1.xyzx));
  vec3 Pf0 = fract(P); // Fractional part for interpolation
  vec3 Pf1 = Pf0 - vec3(1.0); // Fractional part - 1.0
  vec4 ix = vec4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);
  vec4 iy = vec4(Pi0.yy, Pi1.yy);
  vec4 iz0 = Pi0.zzzz;
  vec4 iz1 = Pi1.zzzz;

  vec4 ixy = permute(permute(ix) + iy);
  vec4 ixy0 = permute(ixy + iz0);
  vec4 ixy1 = permute(ixy + iz1);

  vec4 gx0 = ixy0 * (1.0 / 7.0);
  vec4 gy0 = fract(floor(gx0) * (1.0 / 7.0)) - 0.5;
  gx0 = fract(gx0);
  vec4 gz0 = vec4(0.5) - abs(gx0) - abs(gy0);
  vec4 sz0 = step(gz0, vec4(0.0));
  gx0 -= sz0 * (step(0.0, gx0) - 0.5);
  gy0 -= sz0 * (step(0.0, gy0) - 0.5);

  vec4 gx1 = ixy1 * (1.0 / 7.0);
  vec4 gy1 = fract(floor(gx1) * (1.0 / 7.0)) - 0.5;
  gx1 = fract(gx1);
  vec4 gz1 = vec4(0.5) - abs(gx1) - abs(gy1);
  vec4 sz1 = step(gz1, vec4(0.0));
  gx1 -= sz1 * (step(0.0, gx1) - 0.5);
  gy1 -= sz1 * (step(0.0, gy1) - 0.5);

  vec3 g000 = vec3(gx0.x,gy0.x,gz0.x);
  vec3 g100 = vec3(gx0.y,gy0.y,gz0.y);
  vec3 g010 = vec3(gx0.z,gy0.z,gz0.z);
  vec3 g110 = vec3(gx0.w,gy0.w,gz0.w);
  vec3 g001 = vec3(gx1.x,gy1.x,gz1.x);
  vec3 g101 = vec3(gx1.y,gy1.y,gz1.y);
  vec3 g011 = vec3(gx1.z,gy1.z,gz1.z);
  vec3 g111 = vec3(gx1.w,gy1.w,gz1.w);

  vec4 norm0 = taylorInvSqrt(vec4(dot(g000, g000), dot(g010, g010), dot(g100, g100), dot(g110, g110)));
  g000 *= norm0.x;
  g010 *= norm0.y;
  g100 *= norm0.z;
  g110 *= norm0.w;
  vec4 norm1 = taylorInvSqrt(vec4(dot(g001, g001), dot(g011, g011), dot(g101, g101), dot(g111, g111)));
  g001 *= norm1.x;
  g011 *= norm1.y;
  g101 *= norm1.z;
  g111 *= norm1.w;

  float n000 = dot(g000, Pf0);
  float n100 = dot(g100, vec3(Pf1.x, Pf0.yz));
  float n010 = dot(g010, vec3(Pf0.x, Pf1.y, Pf0.z));
  float n110 = dot(g110, vec3(Pf1.xy, Pf0.z));
  float n001 = dot(g001, vec3(Pf0.xy, Pf1.z));
  float n101 = dot(g101, vec3(Pf1.x, Pf0.y, Pf1.z));
  float n011 = dot(g011, vec3(Pf0.x, Pf1.yz));
  float n111 = dot(g111, Pf1);

  vec3 fade_xyz = fade(Pf0);
  vec4 n_z = mix(vec4(n000, n100, n010, n110), vec4(n001, n101, n011, n111), fade_xyz.z);
  vec2 n_yz = mix(n_z.xy, n_z.zw, fade_xyz.y);
  float n_xyz = mix(n_yz.x, n_yz.y, fade_xyz.x); 
  return 2.2 * n_xyz;
}

vec3 spawn_pos(float t, float seed)
{
    return vec3(
        24.0 * (cnoise(vec3(seed, t / 2.0, 26.123))),
        24.0 * (cnoise(vec3(t / 2.0, seed, 51.12))),
        24.0 * (cnoise(vec3(7.123, seed, t / 2.0)))) + vec3(0.0, 20.0, 0.0);
}

void main()
{
    float elapsed = t - old_t;

    vec3 old = spawn_pos(old_t, seed);
    vec3 pos = spawn_pos(t, seed);
    /* vec3 pos = vec3(2.0 * rand(tex * seed), 0.0, 2.0 * rand(tex * seed * 7.3)); */
    /* pos.y = cnoise(vec3(seed, pos.x, pos.z)); */

    /* float hue = ((0.5) * pos.y + 0.5) * 360.0; */

    /* pos.x *= 100.0; */
    /* pos.y = pos.y * 40.0 + 30.0; */
    /* pos.z *= 100.0; */

    vec3 p_rand = rand_spherical(tex * vec2(t, 271.56), 0.6);
    
    float x = pos.x + p_rand.x;
    float y = pos.y + p_rand.y;
    float z = pos.z + p_rand.z;
    /* float x = pos.x; */
    /* float y = pos.y; */
    /* float z = pos.z; */

    gl_FragData[0] = vec4(x, y, z, 0.05 * pos_rand(tex * vec2(t, seed / 1.312)) + 0.05);

    float hue = 360.0 - energy * 16.0;
    hue = max(min(353.9, hue), 6.1);
    hue += rand(tex * vec2(t, seed / 1.85)) * 6.0;

    float sat = pos_rand(tex * vec2(t, seed / 249.435));
    sat = clamp(sat * 2.0, 0.8, 1.0);
    
    float val = pos_rand(tex * vec2(t, seed / 798.23));
    val = clamp(val * 2.0, 0.8, 1.0);
    
    gl_FragData[1] = vec4(hue, sat, val, sign(rand(tex * vec2(t, seed / 368.14)) - 0.8) );

    vec3 vel = pos - vec3(0.0, 20.0, 0.0);// - old;

    float rad = 0.03;
    //if(beat == 0.0)
    //    rad = 5.0;

    vec3 vel_rand = rand_spherical(tex * vec2(t, 98.51212341), rad);

    float xv = 0.02 * vel.x + vel_rand.x;
    float yv = 0.02 * vel.y + vel_rand.y;
    float zv = 0.02 * vel.z + vel_rand.z;
    gl_FragData[2] = vec4(
        xv, yv, zv,
        0.06 * pos_rand(tex * vec2(t, seed / 938.8)) + 0.0025);
    //0.0, 0.0, 0.0, 0.03 * pos_rand(tex * vec2(t * seed / 23.9)) + 0.05);
}
