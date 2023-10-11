#version 430

in layout(location = 0) vec3 position;
in layout(location = 1) vec3 color;
in layout(location = 2) vec2 texcoord;

out layout(location = 0) vec3 ocolor;
out layout(location = 1) vec2 otexcoord;

// pass in model matrix from world3
uniform mat4 model; 
uniform mat4 view;
uniform mat4 projection;

void main()
{
	ocolor = color; // pass color to outcolor
	otexcoord = texcoord;

	mat4 mvp = projection * view * model; // model view projection 
	gl_Position = mvp * vec4(position, 1.0);
}
