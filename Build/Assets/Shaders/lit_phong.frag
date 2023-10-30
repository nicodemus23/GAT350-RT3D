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

in vec3 fposition; // will receive interpolated vertex positions for each fragment 
in vec3 fnormal;
in vec2 ftexcoord;

//in layout(location = 3) vec4 fcolor; 
//flat in layout(location = 2) vec4 fcolor; // "flat" one mormal per polygon, one lighting computuation per polygon

out vec4 ocolor; // this is the pixel we draw to the screen 

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
uniform int numLights = 3;

layout(binding = 0) uniform sampler2D albedoTexture;
layout(binding = 1) uniform sampler2D specularTexture;
layout(binding = 2) uniform sampler2D normalTexture;
layout(binding = 3) uniform sampler2D emissiveTexture;

float attenuation(in vec3 position1, in vec3 position2, in float range)
{
	float distanceSqr = dot(position1 - position2, position1 - position2);
	float rangeSqr = pow(range, 2.0);
	float attenuation = max(0, 1 - pow((distanceSqr / rangeSqr), 2.0));
	attenuation = pow(attenuation, 2.0);
 
	return attenuation;
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
		vec3 reflection = reflect(-lightDir, normal); // calculate reflection vector (which direction light bounces off surface)
		vec3 viewDir = normalize(-position); // calc view dir vector (normalized vector pointing from frag position to camera)
		float intensity = max(dot(reflection, viewDir), 0); // dot product of reflection vector and view direction (angle between ref vector and view vector)
		intensity = pow(intensity, material.shininess); // raise intensity to power of shininess setting in material 
		specular = vec3(intensity * spotIntensity); // final specular color 
	}

}


void main()
{

	vec4 albedoColor = bool(material.params & ALBEDO_TEXTURE_MASK) ? texture(albedoTexture, ftexcoord) : vec4(material.albedo, 1);
	vec4 specularColor = bool(material.params & SPECULAR_TEXTURE_MASK) ? texture(specularTexture, ftexcoord) : vec4(material.specular, 1);
	vec4 emissiveColor = bool(material.params & EMISSIVE_TEXTURE_MASK) ? texture(emissiveTexture, ftexcoord) : vec4(material.emissive, 1);

	// set ambient light // modulated by albedoColor
	ocolor = vec4(ambientLight, 1) * albedoColor + emissiveColor;
 
	// set lights
	for (int i = 0; i < numLights; i++)
	{
		vec3 diffuse;
		vec3 specular;
 
		float attenuation = (lights[i].type == DIRECTIONAL) ? 1 : attenuation(lights[i].position, fposition, lights[i].range);
 
		phong(lights[i], fposition, fnormal, diffuse, specular);
		ocolor += ((vec4(diffuse, 1) * albedoColor) + (vec4(specular, 1)) * specularColor) * lights[i].intensity * attenuation;
	}
}
	