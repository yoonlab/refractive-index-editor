#version 330 core

layout (points) in;
layout (triangle_strip, max_vertices = 16) out;

uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;

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
    vec4 halfBB = gBoundingBox[0] / 2.0 * (1.5 - gColor[0].g) * (1.5 - gColor[0].g);
    vec4 normPos = gl_in[0].gl_Position / gl_in[0].gl_Position.w;

    if (gColor[0].r<0.1)
    {
        //Point C
        gl_Position = normPos + vec4(-1, -1, 1, 0) * halfBB * vec4(0.3, 1, 1, 0);
        EmitVertex();
        //Point A
        gl_Position = normPos + vec4(-1,  1, 1, 0) * halfBB * vec4(0.3, 1, 1, 0);
        EmitVertex();
        //Point D
        gl_Position = normPos + vec4( 1, -1, 1, 0) * halfBB * vec4(0.3, 1, 1, 0);
        EmitVertex();
        //Point B
        gl_Position = normPos + vec4( 1,  1, 1, 0) * halfBB * vec4(0.3, 1, 1, 0);
        EmitVertex();
        EndPrimitive();

        //Point C
        gl_Position = normPos + vec4(-1, -1, 1, 0) * halfBB * vec4(1, 0.3, 1, 0);
        EmitVertex();
        //Point A
        gl_Position = normPos + vec4(-1,  1, 1, 0) * halfBB * vec4(1, 0.3, 1, 0);
        EmitVertex();
        //Point D
        gl_Position = normPos + vec4( 1, -1, 1, 0) * halfBB * vec4(1, 0.3, 1, 0);
        EmitVertex();
        //Point B
        gl_Position = normPos + vec4( 1,  1, 1, 0) * halfBB * vec4(1, 0.3, 1, 0);
        EmitVertex();
        EndPrimitive();
    }
    else
    {
        //Point C
        gl_Position = normPos + vec4(-1, -1, 1, 0) * halfBB * vec4(0.3, 1, 1, 0) + vec4(-1, 0, 0, 0) * halfBB;
        EmitVertex();
        //Point A
        gl_Position = normPos + vec4(-1,  1, 1, 0) * halfBB * vec4(0.3, 1, 1, 0) + vec4(-1, 0, 0, 0) * halfBB;
        EmitVertex();
        //Point D
        gl_Position = normPos + vec4( 1, -1, 1, 0) * halfBB * vec4(0.3, 1, 1, 0) + vec4(-1, 0, 0, 0) * halfBB;
        EmitVertex();
        //Point B
        gl_Position = normPos + vec4( 1,  1, 1, 0) * halfBB * vec4(0.3, 1, 1, 0) + vec4(-1, 0, 0, 0) * halfBB;
        EmitVertex();
        EndPrimitive();

        //Point C
        gl_Position = normPos + vec4(-1, -1, 1, 0) * halfBB * vec4(0.3, 1, 1, 0) + vec4(1, 0, 0, 0) * halfBB;
        EmitVertex();
        //Point A
        gl_Position = normPos + vec4(-1,  1, 1, 0) * halfBB * vec4(0.3, 1, 1, 0) + vec4(1, 0, 0, 0) * halfBB;
        EmitVertex();
        //Point D
        gl_Position = normPos + vec4( 1, -1, 1, 0) * halfBB * vec4(0.3, 1, 1, 0) + vec4(1, 0, 0, 0) * halfBB;
        EmitVertex();
        //Point B
        gl_Position = normPos + vec4( 1,  1, 1, 0) * halfBB * vec4(0.3, 1, 1, 0) + vec4(1, 0, 0, 0) * halfBB;
        EmitVertex();
        EndPrimitive();

        //Point C
        gl_Position = normPos + vec4(-1, -1, 1, 0) * halfBB * vec4(1, 0.3, 1, 0) + vec4(0, -1, 0, 0) * halfBB;
        EmitVertex();
        //Point A
        gl_Position = normPos + vec4(-1,  1, 1, 0) * halfBB * vec4(1, 0.3, 1, 0) + vec4(0, -1, 0, 0) * halfBB;
        EmitVertex();
        //Point D
        gl_Position = normPos + vec4( 1, -1, 1, 0) * halfBB * vec4(1, 0.3, 1, 0) + vec4(0, -1, 0, 0) * halfBB;
        EmitVertex();
        //Point B
        gl_Position = normPos + vec4( 1,  1, 1, 0) * halfBB * vec4(1, 0.3, 1, 0) + vec4(0, -1, 0, 0) * halfBB;
        EmitVertex();
        EndPrimitive();

        //Point C
        gl_Position = normPos + vec4(-1, -1, 1, 0) * halfBB * vec4(1, 0.3, 1, 0) + vec4(0, 1, 0, 0) * halfBB;
        EmitVertex();
        //Point A
        gl_Position = normPos + vec4(-1,  1, 1, 0) * halfBB * vec4(1, 0.3, 1, 0) + vec4(0, 1, 0, 0) * halfBB;
        EmitVertex();
        //Point D
        gl_Position = normPos + vec4( 1, -1, 1, 0) * halfBB * vec4(1, 0.3, 1, 0) + vec4(0, 1, 0, 0) * halfBB;
        EmitVertex();
        //Point B
        gl_Position = normPos + vec4( 1,  1, 1, 0) * halfBB * vec4(1, 0.3, 1, 0) + vec4(0, 1, 0, 0) * halfBB;
        EmitVertex();
        EndPrimitive();
    }
}