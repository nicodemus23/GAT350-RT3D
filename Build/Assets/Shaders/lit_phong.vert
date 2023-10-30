#version 430
// VIEW SPACE (vert is called per vertex)

// vertex attributes
in layout(location = 0) vec3 vposition;
in layout(location = 1) vec2 vtexcoord;
in layout(location = 2) vec3 vnormal;

// attributes passed to fragment shader
out layout(location = 0) vec3 oposition;
out layout(location = 1) vec3 onormal;
out layout(location = 2) vec2 otexcoord;

// pass in model matrix from world - used to transform vertex positions and normals
uniform mat4 model; 
uniform mat4 view;
uniform mat4 projection;

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


void main()
{
	// calculate model-view matrix by * view matrix by model matrix 
	mat4 modelView = view * model;

	// transforms  vertex positions to view space
	oposition = vec3(modelView * vec4(vposition, 1));

	// transforms the normal vectors to view space and normalizes them 
	onormal = normalize(mat3(modelView) * vnormal);

	// calculate texture coordinates, applying tiling and offset as defined in the material struct 
	otexcoord = (vtexcoord * material.tiling) + material.offset;

	// calculates the final position of the vertex in clip space and assigns to gl_Position
	mat4 mvp = projection * view * model; 
	gl_Position = mvp * vec4(vposition, 1.0);
}
