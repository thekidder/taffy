varying vec4 color;
uniform sampler2D tex;

void main()
{
	
	vec4 texel = texture2D(tex, gl_TexCoord[0].st).rgba;
	
	gl_FragColor = color*texel;
}