#version 430
// vert gets called per vertex 
// attributes
in layout(location = 0) vec3 vposition;
in layout(location = 1) vec2 vtexcoord;
in layout(location = 2) vec3 vnormal;

out layout(location = 0) vec2 otexcoord;
out layout(location = 1) vec3 onormal;
out layout(location = 2) vec4 ocolor;

// pass in model matrix from world3
uniform mat4 model; 
uniform mat4 view;
uniform mat4 projection;

uniform struct Material
{
vec4 color;
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
//
//vec3 diffuseLight = vec3(0.75, 0.75, 0.75);
//vec3 lightPosition = vec3(0, 8, 0);

uniform vec3 ambientLight;
uniform vec3 diffuseLight;
uniform vec3 lightPosition;


void main()
{
	otexcoord = (vtexcoord * material.tiling) + material.offset;
	onormal = vnormal;

	// calculating light position 
	mat4 modelView = view * model;
	vec4 position = modelView * vec4(vposition, 1);
	vec3 normal = normalize(mat3(modelView) * vnormal);

	// direction vector 
	vec3 lightDir = normalize(lightPosition - position.xyz); // doing dot product, both need to be normalized or the direction vector doesn't work 
	float intensity = max(dot(lightDir, normal), 0); // max clamps so it never goes less than 0

	vec3 lightColor = (diffuseLight * intensity) + ambientLight;
	ocolor = material.color * vec4(lightColor, 1);

	mat4 mvp = projection * view * model; // model view projection 
	gl_Position = mvp * vec4(vposition, 1.0);
}
