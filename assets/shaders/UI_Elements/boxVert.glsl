#version 330 core
layout (location = 0) in vec2 pos;

uniform vec4 color;
uniform mat4 translate; //position and rotation
uniform vec2 screenDim; //divide to get portion of screen occupied

void main ()
{
	vec4 screenPxl = translate * vec4(pos,0,1);
	vec4 scaleCoords = screenPxl / vec4(screenDim,1,1);
	vec4 screenSpace = scaleCoords*2 + vec4(-1,-1,0,0);
	screenSpace.y = -screenSpace.y;

	gl_Position = screenSpace;
}