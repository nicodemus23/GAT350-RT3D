#version 430

in layout(location = 0) vec3 color;

out layout(location = 0) vec4 ocolor; // this is the pixel we draw to the screen 

uniform float time;

void main()
{
	//vec3 ncolor = vec3(color.r * (sin(time) + 1) * 0.5, color.g * cos(time), color.b * tan(time));
	//vec3 ncolor = vec3((color.r + color.g + color.b) / 3);
	//ocolor = vec4(ncolor, 1);
	ocolor = vec4(color, 1);

}
