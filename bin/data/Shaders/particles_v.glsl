uniform float halfWidth;
uniform mat4 transform;
uniform mat4 lightMatrix;
uniform vec3 lightPos;
uniform float lightIntensity;
uniform vec2 screenSize;

uniform sampler2D shadowMap;
uniform sampler2D colors;
uniform sampler2D positions;

varying vec4 color;

void main() 
{
	vec2 tex = (gl_TextureMatrix[0] * gl_MultiTexCoord0).st;
    vec4 particlePosition = vec4(texture2D(positions, tex).xyz, 1.0);

	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
	gl_Position = gl_ModelViewProjectionMatrix * transform * particlePosition;
    float C = (gl_ProjectionMatrix[0] * halfWidth).x;
	gl_PointSize = gl_Normal.x / (sqrt(1.0 / (C * C)) * gl_Position.z);
    color = texture2D(colors, vec2(gl_Position.x, gl_Position.y)).rgba;

    vec4 shadowCoord = lightMatrix * particlePosition;

    vec2 pixel = vec2(1.0 / screenSize.x, 1.0 / screenSize.y);

    vec4 shadowCoordinateWdivide = shadowCoord / shadowCoord.w ;
	
	/* Used to lower moiré pattern and self-shadowing
	shadowCoordinateWdivide.z += 0.0005;
    */

    float shadow = 1.0;

    float distanceFromLight;

    const float kernel_size = 3.0;
    const float shadow_increment = 2.0 / ((kernel_size * 2.0 + 1.0) * (kernel_size * 2.0 + 1.0));
    if(shadowCoord.w > 0.0)
    {
        for(float i = -kernel_size; i <= kernel_size; i+=1.0)
        {
            for(float j = -kernel_size; j <= kernel_size; j+=1.0)
            {
                distanceFromLight = texture2D(
                    shadowMap,
                    shadowCoordinateWdivide.st + vec2(i * pixel.x,0.0) + vec2(0.0, j * pixel.y)
                    ).z;
                if(distanceFromLight < shadowCoordinateWdivide.z)
                {
                    shadow -= shadow_increment;
                }
            }
        }
    }

    shadow = clamp(shadow, 0.3, 1.0);
    //shadow = 1.0;

    vec3 lightTransformed = (transform * vec4(lightPos, 1.0)).xyz;
    vec3 position = (transform * gl_Vertex).xyz;

    vec3 diff = lightTransformed - position;
    float dist = sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);

    float a = 1.0;
    float b = 0.022;
    float c = 0.002;

    shadow *= lightIntensity / (a + b * dist + c * dist * dist);
    color *= vec4(shadow);
}
