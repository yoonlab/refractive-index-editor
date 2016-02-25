#pragma once

typedef struct {
    float position[3];
    float normal[3];
    float textureCoordinate[2];
} PosNormTexCoordVertex;

typedef struct {
    float position[3];
    float color[3];
} PosColorVertex;
