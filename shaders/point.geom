#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in vec4 gViewPosition[];
in vec4 gColor[];
in vec4 gBoundingBox[];

out vec4 fColor;
out vec4 fViewPosition;

//AB
//CD
//in order of C->A->D->B
//ref. https://en.wikipedia.org/wiki/Triangle_strip

void main()
{
    fViewPosition = gViewPosition[0];
    fColor = gColor[0];
    vec4 halfBB = gBoundingBox[0] / 2.0;
    
    //Point C
    gl_Position = gl_in[0].gl_Position + vec4(-1, -1, 1, 0) * halfBB;
    EmitVertex();

    //Point A
    gl_Position = gl_in[0].gl_Position + vec4(-1,  1, 1, 0) * halfBB;
    EmitVertex();

    //Point D
    gl_Position = gl_in[0].gl_Position + vec4( 1, -1, 1, 0) * halfBB;
    EmitVertex();

    //Point B
    gl_Position = gl_in[0].gl_Position + vec4( 1,  1, 1, 0) * halfBB;
    EmitVertex();

    EndPrimitive();
}