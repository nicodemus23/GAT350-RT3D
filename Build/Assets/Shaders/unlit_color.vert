#version 430

in layout(location = 0) vec3 position;
in layout(location = 1) vec3 color;

out layout(location = 0) vec3 ocolor;

// pass in model matrix from world3
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
	ocolor = color; // pass color to outcolor
	mat4 mvp = projection * view * model; // model view projection 
	gl_Position = mvp * vec4(position, 1.0);
}
