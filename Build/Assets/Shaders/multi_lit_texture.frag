#version 430
// PIXEL SPACE (called per pixel)

// light types
#define POINT 0
#define DIRECTIONAL 1
#define SPOT 2

in vec3 fposition; // will receive interpolated vertex positions for each fragment 
in vec3 fnormal;
in vec2 ftexcoord;

//in layout(location = 3) vec4 fcolor; 
//flat in layout(location = 2) vec4 fcolor; // "flat" one mormal per polygon, one lighting computuation per polygon

out vec4 ocolor; // this is the pixel we draw to the screen 

// this is bound to channel 0
layout(binding = 0) uniform sampler2D tex;

uniform struct Material
{
	vec3 diffuse;
	vec3 specular;
	float shininess;

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

//layout(binding = 0) uniform sampler 2D tex;

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
	diffuse = material.diffuse * (light.color * intensity) * light.intensity;
	


	// SPECULAR lighting component, contributing only if the surface is facing the light 
	specular = vec3(0);
	if (intensity > 0) // checks whether the surface is facing the light source 
	{
		vec3 reflection = reflect(-lightDir, normal); // calculate reflection vector (which direction light bounces off surface)
		vec3 viewDir = normalize(-position); // calc view dir vector (normalized vector pointing from frag position to camera)
		float intensity = max(dot(reflection, viewDir), 0); // dot product of reflection vector and view direction (angle between ref vector and view vector)
		intensity = pow(intensity, material.shininess); // raise intensity to power of shininess setting in material 
		specular = material.specular * intensity * spotIntensity * light.intensity * light.color; // final specular color (lightcolor * (material specular color * intensity)
	}

}

//vec3 ads(in vec3 fposition, in vec3 fnormal)
//{	// AMBIENT lighting component
//	vec3 ambient = ambientLight;
//	uniform numLights = 3;
//
//
//
//	// ATTENUATION
//	float attenuation = 1;
//	if (light.type != DIRECTIONAL)
//	{
//		float distanceSqr = dot(light.position - fposition, light.position - fposition);
//		float rangeSqr = light.range * light.range;
//		attenuation = max(0, 1 - pow((distanceSqr/rangeSqr), 2.0));
//		attenuation = pow(attenuation, 2.0);
//	}
//
//	
//	// returns final light color 
//	return ambient + (diffuse + specular) * light.intensity * attenuation;
//}

void main()
{
	vec4 texcolor = texture(tex, ftexcoord);
	// set ambient light
	ocolor = vec4(ambientLight, 1) * texcolor;
 
	// set lights
	for (int i = 0; i < numLights; i++)
	{
		vec3 diffuse;
		vec3 specular;
 
		float attenuation = (lights[i].type == DIRECTIONAL) ? 1 : attenuation(lights[i].position, fposition, lights[i].range);
 
		phong(lights[i], fposition, fnormal, diffuse, specular);
		ocolor += ((vec4(diffuse, 1) * texcolor) + vec4(specular, 1)) * lights[i].intensity * attenuation;
	}
}
	