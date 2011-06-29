uniform sampler2D texture;
uniform float lightIntensity;

varying vec4 color;
varying float shadow;

void main()
{

	vec4 texel = texture2D(texture, gl_TexCoord[0].st);
	gl_FragColor = shadow * texel;
}
