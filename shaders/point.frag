#version 330 core

in vec4 fColor;
in vec4 fViewPosition;

// Ouput data
out vec3 color;

void main()
{
	color = fColor.rgb;
}