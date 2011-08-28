uniform float halfWidth;
uniform mat4 transform;
uniform mat4 lightMatrix;
uniform vec3 lightPos;
uniform float lightIntensity;
uniform float shadowmap_size;

uniform sampler2D shadowMap;
uniform sampler2D colors;
uniform sampler2D positions;

varying vec4 color;
varying float shadow;
varying float light;
varying float alive;

void main() 
{
	vec2 tex = (gl_TextureMatrix[0] * gl_MultiTexCoord0).st;
    vec4 particlePosition = texture2D(positions, tex);

	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
	gl_Position = gl_ModelViewProjectionMatrix * transform * vec4(particlePosition.xyz, 1.0);
    float Cc = (gl_ProjectionMatrix[0] * halfWidth).x;
	gl_PointSize = particlePosition.w / (sqrt(1.0 / (Cc * Cc)) * gl_Position.z);
    color = texture2D(colors, tex);

    alive = 1.0;
    if(color.a < 0.0)
    {
        // lifetime is < 0, discard
        alive = 0.0;
        return;
    }

    float sat = color.g;
    // convert color from HSV to RGB
    vec3 rgb = vec3(0.0, 0.0, 0.0);
    float h_prime = color.r / 60.0;
    float C = color.b * color.g;
    float X = C * (1.0 - abs(mod(h_prime, 2.0) - 1.0));
    if(0.0 <= h_prime && h_prime < 1.0)
    {
        rgb = vec3(C, X, 0.0);
    }
    else if(1.0 <= h_prime && h_prime < 2.0)
    {
        rgb = vec3(X, C, 0.0);
    }
    else if(2.0 <= h_prime && h_prime < 3.0)
    {
        rgb = vec3(0.0, C, X);
    }
    else if(3.0 <= h_prime && h_prime < 4.0)
    {
        rgb = vec3(0.0, X, C);
    }
    else if(4.0 <= h_prime && h_prime < 5.0)
    {
        rgb = vec3(X, 0.0, C);
    }
    else if(5.0 <= h_prime && h_prime < 6.0)
    {
        rgb = vec3(C, 0.0, X);
    }
    rgb += vec3(color.b - C);
    color = vec4(rgb, color.a);

    vec4 shadowCoord = lightMatrix * vec4(particlePosition.xyz, 1.0);

    vec2 pixel = vec2(1.0 / shadowmap_size, 1.0 / shadowmap_size);

    vec4 shadowCoordinateWdivide = shadowCoord / shadowCoord.w ;
    //shadowCoordinateWdivide.s = 1.0 - shadowCoordinateWdivide.s;	
	// Used to lower moiré pattern and self-shadowing
	// shadowCoordinateWdivide.z += 0.0005;

    shadow = 1.0;

    float distanceFromLight;
    const float kernel_size = 3.0;
    const float shadow_increment = 0.5 / ((kernel_size * 2.0 + 1.0) * (kernel_size * 2.0 + 1.0));
    if(shadowCoord.w > 0.0)
    {
        for(float i = -kernel_size; i <= kernel_size; i+=1.0)
        {
            for(float j = -kernel_size; j <= kernel_size; j+=1.0)
            {
                distanceFromLight = texture2D(
                    shadowMap,
                    shadowCoordinateWdivide.st + vec2(i * pixel.x, j * pixel.y)).z;
                if(distanceFromLight < shadowCoordinateWdivide.z)
                {
                    shadow -= shadow_increment;
                }
            }
        }
    }

    shadow = clamp(shadow, 0.0, 1.0);
    //shadow = 1.0;

    vec3 lightTransformed = lightPos.xyz;
    vec3 position = particlePosition.xyz;

    vec3 diff = lightTransformed - position;
    float dist = sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);

    float a = 1.0;
    float b = 0.002;
    float c = 0.002;

    light = lightIntensity / (a + b * dist + c * dist * dist);
    light = max(light, 2.0 * sat);
}
