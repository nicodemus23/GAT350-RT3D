#version 430
// PIXEL SPACE (called per pixel)

// light types
#define POINT 0
#define DIRECTIONAL 1
#define SPOT 2

in layout(location = 0) vec3 fposition; // will receive interpolated vertex positions for each fragment 
in layout(location = 1) vec3 fnormal;
in layout(location = 2) vec2 ftexcoord;
//in layout(location = 3) vec4 fcolor; 
//flat in layout(location = 2) vec4 fcolor; // "flat" one mormal per polygon, one lighting computuation per polygon

out layout(location = 0) vec4 ocolor; // this is the pixel we draw to the screen 

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

uniform vec3 ambientLight;

uniform struct Light 
{
int type;
vec3 position;
vec3 direction;
vec3 color;
float cutoff;

} light;


vec3 ads(in vec3 fposition, in vec3 fnormal)
{	// ambient lighting component
	vec3 ambient = ambientLight;

	// DIFFUSE lighting component based on the light direction and surface normal 
	vec3 lightDir = (light.type == DIRECTIONAL) ?  normalize(-light.direction) : normalize(light.position - fposition); 
	float intensity = max(dot(lightDir, fnormal), 0); // clamp to 0
	
	float spotIntensity = 1;
	if (light.type == SPOT)
	{
		float angle = acos(dot(light.direction, -lightDir)); // light.direction = direction light is pointing / lightDir is direction from surface we're lighting to the light 
		if (angle > light.cutoff) spotIntensity = 0;
	}

	vec3 diffuse = material.diffuse * (light.color * intensity * spotIntensity);
	


	// SPECULAR lighting component, contributing only if the surface is facing the light 
	vec3 specular = vec3(0);
	if (intensity > 0) // checks whether the surface is facing the light source 
	{
		vec3 reflection = reflect(-lightDir, fnormal); // calculate reflection vector (which direction light bounces off surface)
		vec3 viewDir = normalize(-fposition); // calc view dir vector (normalized vector pointing from frag position to camera)
		intensity = max(dot(reflection, viewDir), 0); // dot product of reflection vector and view direction (angle between ref vector and view vector)
		intensity = pow(intensity, material.shininess); // raise intensity to power of shininess setting in material 
		specular = light.color * (material.specular * intensity); // final specular color (lightcolor * (material specular color * intensity)
	}
	// returns final light color 
	return ambient + diffuse + specular;
}

void main()
{
	// samples a color from the texture based on texture coordinates 
	vec4 texcolor = texture(tex, ftexcoord); 

	// mult texture color with final light color, assigns to output color which is written to framebuffer 
	ocolor = texcolor * vec4(ads(fposition, fnormal), 1);

}
	