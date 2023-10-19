#version 430
// frag gets called per pixel
// indices (index) location numbers need to match those in .vert 
// 
in layout(location = 0) vec2 ftexcoord;
in layout(location = 1) vec3 fnormal;
in layout(location = 2) vec4 fcolor; // "flat" one mormal per polygon, one lighting computuation per polygon
//flat in layout(location = 2) vec4 fcolor; // "flat" one mormal per polygon, one lighting computuation per polygon

out layout(location = 0) vec4 ocolor; // this is the pixel we draw to the screen 

// this is bound to channel 0
layout(binding = 0) uniform sampler2D tex;

void main()
{
	
	vec4 texcolor = texture(tex, ftexcoord); 
	ocolor = texcolor * fcolor; // blend texture color with vertex color

}
	