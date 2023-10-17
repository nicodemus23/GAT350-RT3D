#version 430
// vert gets called per vertex 
// attributes
in layout(location = 0) vec3 vposition;
in layout(location = 1) vec2 vtexcoord;
in layout(location = 2) vec3 vnormal;

out layout(location = 0) vec2 otexcoord;
out layout(location = 1) vec3 onormal;

// pass in model matrix from world3
uniform mat4 model; 
uniform mat4 view;
uniform mat4 projection;

void main()
{
	otexcoord = vtexcoord;
	onormal = vnormal;

	mat4 mvp = projection * view * model; // model view projection 
	gl_Position = mvp * vec4(vposition, 1.0);
}
