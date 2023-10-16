#version 430

//in layout(location = 0) vec3 color;
in layout(location = 1) vec2 texcoord;

out layout(location = 0) vec4 ocolor; // this is the pixel we draw to the screen 


uniform vec4 color;
uniform vec2 offset;
uniform vec2 tiling;

// this is bound to channel 0
layout(binding = 0) uniform sampler2D tex1;
layout(binding = 1) uniform sampler2D tex2;

void main()
{
	
	vec4 texcolor1 = texture(tex1, (texcoord * tiling) + offset); // (sampler2D, UV) 
	vec4 texcolor2 = texture(tex2, (texcoord * tiling) + offset); // (sampler2D, UV) 
	vec4 texcolor = mix(texcolor1, texcolor2, 0.5);

	//if (texcolor.a < 0.8) discard;
	ocolor = texcolor * color; // blend texture color with vertex color

}
	