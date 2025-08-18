#version 330 core

uniform sampler2D tex0;

in vec2 UV;

out vec4 color;

void main ()
{
	color = texture(tex0,UV);
	if (color.w == 0) discard;
}