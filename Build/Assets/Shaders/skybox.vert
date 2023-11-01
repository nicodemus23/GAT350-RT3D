#version 430

in layout(location = 0) vec3 position;

out layout(location = 0) vec3 otexcoord;

// pass in model matrix from world3
uniform mat4 model; 
uniform mat4 view;
uniform mat4 projection;

void main()
{	// only need position for a cubemap 
	otexcoord = position;

	mat4 vp = projection * mat4(mat3(view)); // view, projection - no model since we want to keep it in local and not world space 
	gl_Position = vp * vec4(position, 1.0);
}
