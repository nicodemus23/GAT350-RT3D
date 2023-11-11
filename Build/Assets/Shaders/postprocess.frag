#version 430

#define INVERT_MASK		 (1 << 0)
#define GRAYSCALE_MASK   (1 << 1)
#define	COLORTINT_MASK   (1 << 2)

in layout(location = 0) vec2 ftexcoord;
out layout(location = 0) vec4 ocolor; // this is the pixel we draw to the screen 

uniform float blend = 1;
uniform uint params = 0;

// this is bound to channel 0
layout(binding = 0) uniform sampler2D screenTexture;

vec4 invert(in vec4 color)
{
return  vec4(vec3(1) - color.rgb, color.a);
}

vec4 grayscale(in vec4 color)
{
return vec4(vec3((color.r + color.b + color.g) / 3), color.a);
}
void main()
{
	
	vec4 basecolor = texture(screenTexture, ftexcoord); // (sampler2D, UV) 
	vec4 postprocess = basecolor;

	// 0001 <--mask
	// 0001 <--params

	if (bool(params & INVERT_MASK)) postprocess = invert(postprocess);
	if (bool(params & GRAYSCALE_MASK)) postprocess = grayscale(postprocess);
	ocolor = mix(basecolor, postprocess, blend);

}