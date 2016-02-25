#pragma once

#include "Common.h"
#include "GpuProgram.h"
#include "SceneNode.h"

class Curve : public SceneNode
{
public:
    Curve(const std::string *name,
        std::vector<PosColorVertex> *vertexData);
    virtual ~Curve();

    void glInit();
    void draw();
    void updateVertices(std::vector<PosColorVertex> *vertexData);

protected:
    std::vector<PosColorVertex> vertexData;
    std::vector<GLuint> indices;

    bool isVertexAttribArrayEnabled;
};