uniform sampler2D texture;

void main()
{
	vec4 texel = texture2D(texture, gl_TexCoord[0].st);
	gl_FragColor = texel;
}
