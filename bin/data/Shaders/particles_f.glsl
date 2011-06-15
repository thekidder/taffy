//uniform sampler2D color;
uniform sampler2D texture;

varying vec2 tex;
varying vec4 color;

void main()
{
	//vec4 pColor = vec4(texture2D(color, tex).rgb, 1.0);
	vec4 texel = texture2D(texture, gl_TexCoord[0].st);
	gl_FragColor = color*texel;
}
