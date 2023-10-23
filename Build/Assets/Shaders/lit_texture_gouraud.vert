#version 430
// vert gets called per vertex 
// attributes
in layout(location = 0) vec3 vposition;
in layout(location = 1) vec2 vtexcoord;
in layout(location = 2) vec3 vnormal;

out layout(location = 0) vec3 oposition;
out layout(location = 1) vec3 onormal;
out layout(location = 2) vec2 otexcoord;
out layout(location = 3) vec4 ocolor;

// pass in model matrix from world3
uniform mat4 model; 
uniform mat4 view;
uniform mat4 projection;

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
vec3 position;
vec3 color;

} light;

// IN CLASS 10.18.2023 // hard coded values 
//vec3 ambientLight = vec3(0.1, 0.1, 0.1);
//vec3 diffuseLight = vec3(0.75, 0.75, 0.75);
//vec3 lightPosition = vec3(0, 8, 0);

uniform vec3 ambientLight;
//vec3 diffuseLight;
//vec3 lightPosition;

vec3 ads(in vec3 position, in vec3 normal)
{	// ambient
	vec3 ambient = ambientLight;

	// diffuse 
	vec3 lightDir = normalize(light.position - position); 
	float intensity = max(dot(lightDir, normal), 0); // clamp to 0
	vec3 diffuse = material.diffuse * (light.color * intensity);

	// specular
	vec3 specular = vec3(0);
	if (intensity > 0)
	{
		vec3 reflection = reflect(-lightDir, normal); // reflect(negative angle of light ray, normal to reflect about)
		vec3 viewDir = normalize(-position);
		intensity = max(dot(reflection, viewDir), 0);
		intensity = pow(intensity, material.shininess);
		specular = material.specular * intensity;
	}

	return ambient + diffuse + specular;
}


void main()
{
	mat4 modelView = view * model;

	// convert position and normal to world-view space 
	otexcoord = (vtexcoord * material.tiling) + material.offset;
	onormal = normalize(mat3(modelView) * vnormal);
	oposition = vec3(modelView * vec4(vposition, 1));

	
	ocolor = vec4(ads(oposition, onormal), 1);

	mat4 mvp = projection * view * model; // model view projection 
	gl_Position = mvp * vec4(vposition, 1.0);
}
