#include "Curve.h"
#include "PhysicsInterface.h"

Curve::Curve(const std::string * name,
    std::vector<PosColorVertex> *vertexData)
    : SceneNode(name, GL_LINE, &glm::mat4()),
    isVertexAttribArrayEnabled(false)
{
    this->vertexData = std::vector<PosColorVertex>(*vertexData);
}

Curve::~Curve()
{
    vertexData.clear();
}

void Curve::glInit()
{
    indices.clear();
    for (auto position : vertexData)
    {
        indices.push_back(indices.size());
    }
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(PosColorVertex) * vertexData.size(), &vertexData[0], GL_DYNAMIC_DRAW);
    if (!isVertexAttribArrayEnabled)
    {
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(PosColorVertex), (void*)0);                       //send positions on pipe 0
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(PosColorVertex), (void*)(sizeof(float) * 3));     //send colors on pipe 1
        isVertexAttribArrayEnabled = true;
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices[0], GL_DYNAMIC_DRAW);
    glBindVertexArray(0);
}

void Curve::updateVertices(std::vector<PosColorVertex>* vertexData)
{
    this->vertexData = std::vector<PosColorVertex>(*vertexData);
    glInit();
}

void Curve::draw()
{
    bindBuffers();
    glDrawArrays(primitiveMode, 0, vertexData.size());
    unbindBuffers();
}
