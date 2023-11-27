#version 430
// PIXEL SPACE (called per pixel)

// light types
#define POINT		0
#define DIRECTIONAL 1
#define SPOT		2

#define ALBEDO_TEXTURE_MASK		(1 << 0) 
#define SPECULAR_TEXTURE_MASK	(1 << 1)
#define NORMAL_TEXTURE_MASK		(1 << 2)
#define EMISSIVE_TEXTURE_MASK	(1 << 3)
#define	EDGE_DETECTION_MASK		(1 << 8)


in layout(location = 0) vec3 fposition; // will receive interpolated vertex positions for each fragment 
in layout(location = 1) vec3 fnormal;
in layout(location = 2) vec2 ftexcoord;
in layout(location = 3) vec4 fshadowcoord;

//in layout(location = 3) vec4 fcolor; 
//flat in layout(location = 2) vec4 fcolor; // "flat" one mormal per polygon, one lighting computuation per polygon

out layout(location = 0) vec4 ocolor; // this is the pixel we draw to the screen 

// edge detection:
uniform float edgeIntensity = 0.0;

uniform struct Material
{
	uint params;
	vec3 albedo;
	vec3 specular;
	float shininess;
	vec3 emissive;

	vec2 offset;
	vec2 tiling;
} material;


uniform struct Light 
{
int type;
vec3 position;
vec3 direction;
vec3 color;
float intensity;
float range;
float innerAngle;
float outerAngle;

} lights[3];

uniform vec3 ambientLight;
uniform int numLights = 3;
uniform float shadowBias = 0.005;
uniform int stepCount;

layout(binding = 0) uniform sampler2D albedoTexture;
layout(binding = 1) uniform sampler2D specularTexture;
layout(binding = 2) uniform sampler2D normalTexture;
layout(binding = 3) uniform sampler2D emissiveTexture;
layout(binding = 5) uniform sampler2D shadowTexture;
layout(binding = 6) uniform sampler2D depthMap;

// define edge detection kernel size: 
const float edgeKernel[25] = float[](
	 -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1,
    -1, -1, 24, -1, -1,
    -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1
);
const int edgeKernelSize = 5;

vec4 applyEdgeKernel(in vec2 uv, in vec2 texelSize, float intensity) {

    if (intensity == 0) return texture(shadowTexture, uv);

    vec4 edgeColor = vec4(0.0);
    float depthValue;
    float kernelResult;
    for(int y = -2; y <= 2; y++) {
        for(int x = -2; x <= 2; x++) {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            depthValue = texture(shadowTexture, uv + offset).r;
            kernelResult = edgeKernel[(y + 2) * 5 + (x + 2)];
            edgeColor.r += depthValue * kernelResult;
            edgeColor.g += depthValue * kernelResult;
            edgeColor.b += depthValue * kernelResult;
        }
    }
	// normalize the result to increase edge strength:
	//edgeColor.rgb = abs(edgeColor.rgb); // Take the absolute value to visualize edges
    //edgeColor.rgb = clamp(edgeColor.rgb, 0.0, 1.0); // Clamp values to valid range
	edgeColor.rgb = abs(edgeColor.rgb);
    edgeColor.rgb = edgeColor.rgb / (max(edgeColor.r, max(edgeColor.g, edgeColor.b)));


    //edgeColor *= intensity; // Scale by intensity
	edgeColor.rgb = vec3(1.0) * intensity * step(0.1, edgeColor.r); // threshold
    edgeColor.a = 1.0; // Set alpha to 1.0

	return edgeColor;
	}

float attenuation(in vec3 position1, in vec3 position2, in float range)
{
	float distanceSqr = dot(position1 - position2, position1 - position2);
	float rangeSqr = pow(range, 2.0);
	float attenuation = max(0, 1 - pow((distanceSqr / rangeSqr), 2.0));
	attenuation = pow(attenuation, 2.0);
 
	return attenuation;
}

float calculateShadow(vec4 shadowcoord, float bias)
{
	return texture(shadowTexture, shadowcoord.xy).x < shadowcoord.z  - shadowBias ? 0 : 1; // zero, you're in the shadow ,1 means you're in light 100%
}
void phong(in Light light, in vec3 position, in vec3 normal, out vec3 diffuse)
{
	// DIFFUSE lighting component based on the light direction and surface normal 
	vec3 lightDir = (light.type == DIRECTIONAL) ?  normalize(-light.direction) : normalize(light.position - position); 
	
	float spotIntensity = 1;
	if (light.type == SPOT)
	{
		float angle = acos(dot(light.direction, -lightDir)); // light.direction = direction light is pointing / lightDir is direction from surface we're lighting to the light 
		//if (angle > light.innerAngle) spotIntensity = 0;
		spotIntensity = smoothstep(light.outerAngle + 0.001, light.innerAngle, angle);
	}
	// 
	float intensity = max(dot(lightDir, normal), 0) * spotIntensity;
	// intensity quantized into number of steps (stepCount). Divide into ranges based on stepCount
	float stepSize = 1.0 / float (stepCount);
	intensity = floor(intensity / stepSize) * stepSize;
	diffuse = (light.color * intensity); // still diffuse lighting but albedo texture

}


void main()
{
	// ternary operations 
	vec4 albedoColor = bool(material.params & ALBEDO_TEXTURE_MASK) ? texture(albedoTexture, ftexcoord) : vec4(material.albedo, 1);
	//vec4 specularColor = bool(material.params & SPECULAR_TEXTURE_MASK) ? texture(specularTexture, ftexcoord) : vec4(material.specular, 1);
	vec4 emissiveColor = bool(material.params & EMISSIVE_TEXTURE_MASK) ? texture(emissiveTexture, ftexcoord) : vec4(material.emissive, 1);

	// calculate texel size for the depth map:
	vec2 texelSize = 1.0 / vec2(textureSize(depthMap, 0));

	// apply edge detection:
	vec4 edgeResult = vec4(0.0);
	 if (bool(material.params & EDGE_DETECTION_MASK)) {
        edgeResult = applyEdgeKernel(ftexcoord, texelSize, edgeIntensity);
    }

	// set ambient light + emissive color // modulated by albedoColor
	vec4 ambientEmissive = vec4(ambientLight, 1) * albedoColor + emissiveColor;

	// calculate shadow: 
	//float shadow = calculateShadow(fshadowcoord, shadowBias);

	// Initialize final color with ambient and emissive contributions
    vec4 finalColor = ambientEmissive;

 
	// set lights
	for (int i = 0; i < numLights; i++)
	{
		vec3 diffuse;
		//vec3 specular;
 
		float attenuation = (lights[i].type == DIRECTIONAL) ? 1 : attenuation(lights[i].position, fposition, lights[i].range);
 
		phong(lights[i], fposition, fnormal, diffuse);
		finalColor += (vec4(diffuse, 1) * albedoColor) * lights[i].intensity * attenuation;
		}
	

	// calculate/apply shadow:
	finalColor *= calculateShadow(fshadowcoord, shadowBias);

	if (edgeResult.r > 0.0) {
		// if edge is detected, use the edge color (white)
		ocolor = mix(finalColor, vec4(1.0), edgeResult.r * edgeIntensity); // Blends scene color with white based on edge detection;
		} else {
		// no edge detected, use the lighting result:
        //finalColor = mix(finalColor, edgeResult, edgeResult.a);
		ocolor += finalColor;
		

    }

	// output final color:
	ocolor = clamp(finalColor, 0.0, 1.0);


	// blend edge detection result with final color:

}

//void phong(in Light light, in vec3 position, in vec3 normal, out vec3 diffuse, out vec3 specular)
//{
//	// DIFFUSE lighting component based on the light direction and surface normal 
//	vec3 lightDir = (light.type == DIRECTIONAL) ?  normalize(-light.direction) : normalize(light.position - position); 
//	
//	float spotIntensity = 1;
//	if (light.type == SPOT)
//	{
//		float angle = acos(dot(light.direction, -lightDir)); // light.direction = direction light is pointing / lightDir is direction from surface we're lighting to the light 
//		//if (angle > light.innerAngle) spotIntensity = 0;
//		spotIntensity = smoothstep(light.outerAngle + 0.001, light.innerAngle, angle);
//	}
//	// 
//	float intensity = max(dot(lightDir, normal), 0) * spotIntensity;
//	if (intensity > 0.5) intensity = 1.0;
//	else intensity = 0.5;
//	diffuse = (light.color * intensity); // still diffuse lighting but albedo texture
//	
//
//
//	// SPECULAR lighting component, contributing only if the surface is facing the light 
//	specular = vec3(0);
//	if (intensity > 0) // checks whether the surface is facing the light source 
//	{
//
//		vec3 viewDir = normalize(-position); // calc view dir vector (normalized vector pointing from frag position to camera)
//		// phong:
//		//vec3 reflection = reflect(-lightDir, normal); // calculate reflection vector (which direction light bounces off surface)
//		//float intensity = max(dot(reflection, viewDir), 0); // dot product of reflection vector and view direction (angle between ref vector and view vector)
//
//		// blinn-phong:
//		vec3 h = normalize(viewDir + lightDir); // normalize makes it a unit vector 
//		intensity = max(dot(h, normal), 0);
//
//		intensity = pow(intensity, material.shininess); // raise intensity to power of shininess setting in material 
//		specular = vec3(intensity * spotIntensity); // final specular color 
//	}
//
//	