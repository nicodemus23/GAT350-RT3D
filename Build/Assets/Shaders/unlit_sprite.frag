#version 430

in layout(location = 0) vec2 ftexcoord;

out layout(location = 0) vec4 ocolor; // this is the pixel we draw to the screen 

uniform struct Material
{
	vec3 albedo;
	
	vec2 offset;
	vec2 tiling;
} material;


// this is bound to channel 0
layout(binding = 0) uniform sampler2D tex;

void main()
{
	vec4 texcolor = texture(tex, ftexcoord);// (sampler2D, UV) 

	if (texcolor.a < 0.5) discard;
	ocolor = texcolor * vec4(material.albedo, 1); // blend texture color with vertex color

}
	