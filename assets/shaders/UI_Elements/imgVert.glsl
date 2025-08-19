#version 330 core
layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 UVin;

uniform mat4 translate; //position and rotation
uniform vec2 screenDim; //divide to get portion of screen occupied

out vec2 UV;

void main ()
{
	vec4 screenPxl = translate * vec4(pos,0,1);
	float zResult = screenPxl.z;
	vec4 scaleCoords = screenPxl / vec4(screenDim,1,1);
	vec4 screenSpace = (scaleCoords*vec4(2,2,1,1) + vec4(-1,-1,0,0));
	screenSpace.y = -screenSpace.y;

	gl_Position = vec4(screenSpace.xy,tanh(zResult * 0.2),1);

	UV = UVin;
}