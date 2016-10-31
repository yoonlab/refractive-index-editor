#version 330 core

in vec3 in_Position;
in vec2 in_UV;

out vec4 pos;
out vec2 texCoord;

void main(void)
{
    pos         = vec4(in_Position, 1.0);
    gl_Position = pos;
    texCoord    = in_UV;
}