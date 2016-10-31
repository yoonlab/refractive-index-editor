#version 330 core

in vec3 in_Position;
in vec3 in_Color;

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;

out vec4 gViewPosition;
out vec4 gColor;
out vec4 gBoundingBox;

vec2 boundingBox = vec2(0.02, 0.02);

void main(void)
{
    gl_Position = ProjectionMatrix * ViewMatrix * vec4(in_Position, 1.0);
    gViewPosition = ViewMatrix * vec4(in_Position, 1.0);
    gColor = vec4(in_Color, 1.0);
    gBoundingBox = ProjectionMatrix * vec4(boundingBox, 0.0, 0.0);
}