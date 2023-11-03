#version 430
// attributes
in layout(location = 0) vec3 vposition;
in layout(location = 2) vec3 vnormal;

out layout(location = 0) vec3 otexcoord;

uniform float ior = 1.55;

// pass in model matrix from world3
uniform mat4 model; 
uniform mat4 view;
uniform mat4 projection;

vec3 vreflect(vec3 i, vec3 n) // incident vector, normal
{
	return i - (n * dot(n, i)) * 2;
}

void main()
{	
// transform vertex position/normal to world space - view independent 
	vec3 position = vec3(model * vec4(vposition, 1)); // 1 adds position, 0 rids of it 
	vec3 normal = normalize(mat3(model) * vnormal);  // normalize puts back to unit vector

	// Inverse of view space-> puts in world coords
	// last column of mat4 is position = returns just position 
	vec3 viewPosition = inverse(view)[3].xyz; 

	// head minus tail: position - viewPosition // normalized vector pointing from the view position to the vertex position
	vec3 viewDir = normalize(position - viewPosition); // normalize makes unit vector = 1

	// normal = normalized normal vector at the vertex // 1.0/ior = reciprocal of index of refraction 
	// calculation of refraction vector is then assigned to otexcoord which is passed to .frag so it knows how to texture the vertex 
	otexcoord = refract(viewDir, normal, 1.0 / ior);

	mat4 vp = projection * view * model; // view, projection - no model since we want to keep it in local and not world space 
	gl_Position = vp * vec4(vposition, 1.0);
}
