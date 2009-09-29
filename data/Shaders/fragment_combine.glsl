uniform sampler2D unblur;
uniform sampler2D blur;

void main()
{
  vec4 texel;
  texel  = texture2D(blur,   gl_TexCoord[0].st);
  texel += texture2D(unblur, gl_TexCoord[0].st);
  gl_FragColor = texel;
}