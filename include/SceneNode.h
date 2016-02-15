#ifndef _SCENE_NODE_H_
#define _SCENE_NODE_H_

#include "Common.h"
#include "Material.h"
#include "GpuProgram.h"

class SceneNode
{
public:
    SceneNode();
    SceneNode(const std::string *name,
        GLenum primitiveMode,
        glm::mat4 *modelViewMatrix);
    virtual ~SceneNode();

    virtual void glInit() = 0;
    virtual void draw() = 0;

    const glm::mat4 getModelViewMatrix() { return modelViewMatrix; }
    const GLenum getPrimitiveMode() { return primitiveMode; }

protected:
    void bindBuffers();
    void unbindBuffers();

    std::string name;
    glm::mat4 modelViewMatrix;
    GLenum primitiveMode;
    
    GLuint vao, vbo, ibo;
};

#endif
