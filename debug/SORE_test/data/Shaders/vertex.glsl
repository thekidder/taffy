varying vec4 diffuse,ambientGlobal,ambient;
varying vec3 normal,lightDir,halfVector;
varying float dist;

void main() 
{
	vec4 ecPos;
	vec3 aux;
	
	normal = normalize(gl_NormalMatrix * gl_Normal);
	
	ecPos = gl_ModelViewMatrix * gl_Vertex;
	aux = vec3(gl_LightSource[0].position-ecPos);
	lightDir = normalize(aux);
	dist = length(aux);

	halfVector = normalize(gl_LightSource[0].halfVector.xyz);
	
	diffuse = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;
	
	ambient = gl_FrontMaterial.ambient * gl_LightSource[0].ambient;
	ambientGlobal = gl_LightSource[0].ambient * gl_FrontMaterial.ambient * gl_Color;

	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_TexCoord[1] = gl_MultiTexCoord1;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}