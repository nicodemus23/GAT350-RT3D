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

in layout(location = 0) vec3 fposition; // will receive interpolated vertex positions for each fragment 
in layout(location = 1) vec2 ftexcoord;
in layout(location = 2) mat3 ftbn;
in layout(location = 5) vec4 fshadowcoord;


out layout(location = 0) vec4 ocolor; // this is the pixel we draw to the screen 

// this is bound to channel 0

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
uniform float ambientIntensity;
uniform float shadowBias = 0.005;
uniform float normalMapIntensity = 1.0;
uniform float normalMapContrast = 1.0;

uniform int numLights = 3;

layout(binding = 0) uniform sampler2D albedoTexture;
layout(binding = 1) uniform sampler2D specularTexture;
layout(binding = 2) uniform sampler2D normalTexture;
layout(binding = 3) uniform sampler2D emissiveTexture;
layout(binding = 5) uniform sampler2D shadowTexture;


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

void phong(in Light light, in vec3 position, in vec3 normal, out vec3 diffuse, out vec3 specular)
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

	float intensity = max(dot(lightDir, normal), 0) * spotIntensity;
	diffuse = (light.color * intensity); // still diffuse lighting but albedo texture
	


	// SPECULAR lighting component, contributing only if the surface is facing the light 
	specular = vec3(0);
	if (intensity > 0) // checks whether the surface is facing the light source 
	{
		vec3 viewDir = normalize(-position); // calc view dir vector (normalized vector pointing from frag position to camera)
		//vec3 reflection = reflect(-lightDir, normal); // calculate reflection vector (which direction light bounces off surface)
		//float intensity = max(dot(reflection, viewDir), 0); // dot product of reflection vector and view direction (angle between ref vector and view vector)

		// blinn-phong (spec change mainly)
		vec3 h = normalize(viewDir + lightDir); // normalize makes it a unit vector 
		intensity = max(dot(h, normal), 0);

		intensity = pow(intensity, material.shininess); // raise intensity to power of shininess setting in material 
		specular = vec3(intensity * spotIntensity); // final specular color 
	}

}


void main()
{
	// ternary operations 
	vec4 albedoColor = bool(material.params & ALBEDO_TEXTURE_MASK) ? texture(albedoTexture, ftexcoord) : vec4(material.albedo, 1);
	vec4 specularColor = bool(material.params & SPECULAR_TEXTURE_MASK) ? texture(specularTexture, ftexcoord) : vec4(material.specular, 1);
	vec4 emissiveColor = bool(material.params & EMISSIVE_TEXTURE_MASK) ? texture(emissiveTexture, ftexcoord) : vec4(material.emissive, 1);

	// set ambient light + emissive color // modulated by albedoColor and ambientIntensity (ambientIntensity is my addition)
	ocolor = vec4(ambientLight * ambientIntensity, 1) * albedoColor + emissiveColor;
	float shadow = calculateShadow(fshadowcoord, shadowBias);

 
	// set lights
	for (int i = 0; i < numLights; i++)
	{
		vec3 diffuse;
		vec3 specular;
 
		float attenuation = (lights[i].type == DIRECTIONAL) ? 1 : attenuation(lights[i].position, fposition, lights[i].range);

		// get normal from the normal map
		vec3 normal = texture(normalTexture, ftexcoord).rgb;
		// transform normal map from [0, 1] to [-1, 1]
		normal = (normal * 2 - 1.0); // new value = original value * 2 - 1 // -1 ->1 is suitable for normal lighting calculations // -1 = normal pointing negative direction to surface, 0 = perp. to surface, 1 = positive direction
		normal = mix(vec3(0.5), normal, normalMapContrast); // apply contrast 
		normal = normal * normalMapIntensity; // apply intensity
		// normalize the normal
		normal = normalize(ftbn * normal); // transform to world space 

		// DEBUG NORMAL MAP: 
		//ocolor = vec4(normal * 0.5 + 0.5, 1); // Convert from [-1,1] to [0,1] range for display

		// Visualize the normalMapIntensity by setting it as the red channel
		// and displaying the modified normal in the green and blue channels:
		//ocolor = vec4(normalMapIntensity, normal.yz * 0.5 + 0.5, 1);

		// Visualize the intensity as a grayscale color:
		//ocolor = vec4(vec3(normalMapIntensity), 1.0);
 
		// regular output:
		phong(lights[i], fposition, normal, diffuse, specular);
		ocolor += ((vec4(diffuse, 1) * albedoColor) + (vec4(specular, 1)) * specularColor) * lights[i].intensity * attenuation * shadow;
	}
	// outputs just the normal map for viewing 
	//ocolor = vec4(texture(normalTexture, ftexcoord).rgb, 1);
}
	