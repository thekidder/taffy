uniform float lightIntensity;

varying vec4 color;
varying vec3 lightTransformed;
varying vec3 position;

void main()
{
    vec3 diff = lightTransformed - position;
    float dist = sqrt(diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);

    float a = 1.0;
    float b = 0.0022;
    float c = 0.002;

    float light = lightIntensity / (a + b * dist + c * dist * dist);

	gl_FragColor = light * color;
}
