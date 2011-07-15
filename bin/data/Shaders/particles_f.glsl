uniform sampler2D texture;
uniform float lightIntensity;

varying vec4 color;
varying float shadow;

void main()
{
	vec4 texel = texture2D(texture, gl_TexCoord[0].st);
    if(texel.a < 0.5)
        discard;

    if(texel.a < 1.0)
        texel = vec4(1.0, 1.0, 1.0, texel.a);

    texel.a = 1.0;

	gl_FragColor = shadow * texel;
}
