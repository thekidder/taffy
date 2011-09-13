uniform float lightIntensity;
uniform sampler2D shadowMap;
uniform mat4 lightMatrix;
uniform vec3 lightPos;
uniform float shadowmap_size;

varying vec4 color;
varying vec3 position;
varying vec4 position_transform;

void main()
{
    /* float kernel[] = { */
    /*     0.0042, 0.0078, 0.0114, 0.0129, 0.0114, 0.0078, 0.0042, */
    /*     0.0078, 0.0146, 0.0213, 0.0241, 0.0213, 0.0146, 0.0078, */
    /*     0.0114, 0.0213, 0.0310, 0.0351, 0.0310, 0.0213, 0.0114, */
    /*     0.0129, 0.0241, 0.0351, 0.0398, 0.0351, 0.0241, 0.0129, */
    /*     0.0114, 0.0213, 0.0310, 0.0351, 0.0310, 0.0213, 0.0114, */
    /*     0.0078, 0.0146, 0.0213, 0.0241, 0.0213, 0.0146, 0.0078, */
    /*     0.0042, 0.0078, 0.0114, 0.0129, 0.0114, 0.0078, 0.0042}; */


    vec3 diff = lightPos - position;
    float dist = sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);

    float a = 1.0;
    float b = 0.0006;
    float c = 0.0005;

    float light = lightIntensity / (a + b * dist + c * dist * dist);
    float shadow = 1.0;

    vec4 shadowCoord = lightMatrix * vec4(position, 1.0);
    vec2 pixel = vec2(1.0 / shadowmap_size, 1.0 / shadowmap_size);
    vec4 shadowCoordinateWdivide = shadowCoord / shadowCoord.w ;

    shadowCoordinateWdivide.s = 1.0 - shadowCoordinateWdivide.s;
	
	// Used to lower moiré pattern and self-shadowing
    shadowCoordinateWdivide.z += 0.0005;

    float distanceFromLight;
    const int kernel_size = 3;
    const float shadow_increment = 0.5 / ((kernel_size * 2.0 + 1.0) * (kernel_size * 2.0 + 1.0));
    if(shadowCoord.w > 0.0)
    {
        for(int i = -kernel_size; i <= kernel_size; ++i)
        {
            for(int j = -kernel_size; j <= kernel_size; ++j)
            {
                if(shadowCoordinateWdivide.s < 0.05 || shadowCoordinateWdivide.s > 0.95 ||
                   shadowCoordinateWdivide.t < 0.05 || shadowCoordinateWdivide.t > 0.95)
                    continue;

                distanceFromLight = texture2D(
                    shadowMap,
                    shadowCoordinateWdivide.st + vec2(i * pixel.x, j * pixel.y)).z;
                if(distanceFromLight < shadowCoordinateWdivide.z)
                {
                    //shadow -= kernel[(kernel_size * 2 + 1) * (j + kernel_size) + (i + kernel_size)];
                    shadow -= shadow_increment;
                }
            }
        }
    }

	gl_FragColor = shadow * light * color;
}
