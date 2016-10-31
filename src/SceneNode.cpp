#include "SceneNode.h"
#include "PhysicsInterface.h"

SceneNode::SceneNode()
{
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ibo);
}

SceneNode::SceneNode(const std::string * name,
    GLenum primitiveMode)
    : SceneNode()
{
    this->name = std::string(*name);
    this->primitiveMode = primitiveMode;
}

SceneNode::SceneNode(const std::string * name,
    GLenum primitiveMode,
    glm::mat4 * modelViewMatrix)
    : SceneNode()
{
    this->name = std::string(*name);
    this->primitiveMode = primitiveMode;
    this->modelViewMatrix = glm::mat4(*modelViewMatrix);
}

SceneNode::~SceneNode()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);
}

void SceneNode::bindBuffers()
{
    glBindVertexArray(vao);
}

void SceneNode::unbindBuffers()
{
    glBindVertexArray(0);
}