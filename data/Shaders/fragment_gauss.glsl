uniform sampler2D source;
uniform float offsetx;
uniform float offsety;

void main()
{
  vec4 color;
  vec2 offset = vec2(offsetx, offsety);
  vec2 tex = gl_TexCoord[0].st;
  
  //gaussian blur with sigma = 1.1
  color  = 1.0 * texture2D(source, tex - offset*4);
  color += 4.0 * texture2D(source, tex - offset*3);
  color += 6.0 * texture2D(source, tex - offset*2);
  color += 8.0 * texture2D(source, tex - offset*1);
  color += 10.0 * texture2D(source, tex);
  color += 8.0 * texture2D(source, tex + offset*1);
  color += 6.0 * texture2D(source, tex + offset*2);
  color += 4.0 * texture2D(source, tex + offset*3);
  color += 1.0 * texture2D(source, tex + offset*4);
  color /= 38.0f;
  color.a = 1.0f;
  gl_FragColor = color;
}