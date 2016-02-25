#pragma once

#include "Common.h"
#include "VertexTypes.h"
#include "GpuProgram.h"
#include "SceneNode.h"

class PointIndicator : public SceneNode
{
public:
    PointIndicator(const std::string *name,
        std::vector<PosColorVertex> *vertexData);
    virtual ~PointIndicator();

    void glInit();
    void draw();
    void updateVertices(std::vector<PosColorVertex> *vertexData);

protected:
    std::vector<PosColorVertex> vertexData;
    std::vector<GLuint> indices;

    bool isVertexAttribArrayEnabled;
};