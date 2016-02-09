#ifndef _SCENE_NODE_H_
#define _SCENE_NODE_H_

#include "Common.h"
#include "Material.h"
#include "GpuProgram.h"

#include <btBulletDynamicsCommon.h>

typedef struct {
    float top, bottom, left, right, front, back;
} BoundingBox;

class SceneNode
{
public:
    SceneNode();
    SceneNode(const std::string *name,
        const std::string *material,
        std::vector<Vertex> *vertexData,
        GLenum primitiveMode,
        glm::mat4 *modelViewMatrix);
    virtual ~SceneNode();

    BoundingBox* getBoundingBox();
    void calcBoundingSphere();
    void glInit();
    void bindBuffers();
    void unbindBuffers();

    const std::string getMaterialName() { return material; }
    GLuint *getDiffuseTextureIdPtr() { return &diffuseTextureId; }
    const glm::vec4 getBoundingSphereCenter() { return glm::vec4(lx, ly, lz, 1.f); }
    const GLfloat getBoundingSphereRadius() { return boundingSphere; }
    const glm::mat4 getModelViewMatrix() { return modelViewMatrix; }
    const GLenum getPrimitiveMode() { return primitiveMode; }
    const GLuint getVertexLength() { return vertexData.size(); }
    btCollisionObject *getCollisionObjectPtr() { return collisionObject; }

private:
    std::string name;
    std::string material;
    std::vector<Vertex> vertexData;
    std::vector<GLuint> indices;
    glm::mat4 modelViewMatrix;
    GLenum primitiveMode;

    GLuint ambientTextureId;
    GLuint diffuseTextureId;
    GLuint normalTextureId;
    GLuint specularTextureId;

    GLfloat boundingSphere;
    GLfloat lx, ly, lz;
    BoundingBox *bbox;

    GLuint vao, vbo, ibo;

    btCollisionObject *collisionObject;
};

#endif
