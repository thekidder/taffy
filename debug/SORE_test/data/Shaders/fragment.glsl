varying vec4 diffuse,ambientGlobal, ambient;
varying vec3 normal,lightDir,halfVector;
varying float dist;
uniform sampler2D grass;
uniform sampler2D texture;

void main()
{
	vec4 texel;
	texel = texture2D(grass,gl_TexCoord[0].st).rgba;
	vec4 texel2 = texture2D(texture, gl_TexCoord[1].st);

	vec3 n;
	float NdotL;
	vec4 color = ambientGlobal;
	float att;
	
	n = normalize(normal);
	
	NdotL = max(dot(n,normalize(lightDir)),0.0);

	att = 1.0 / (gl_LightSource[0].constantAttenuation +
		gl_LightSource[0].linearAttenuation * dist +
		gl_LightSource[0].quadraticAttenuation * dist * dist);
	color += att * (diffuse * NdotL + ambient);


	gl_FragColor = color * (texel);
}