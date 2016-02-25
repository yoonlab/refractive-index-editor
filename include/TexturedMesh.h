#pragma once

#include "Common.h"
#include "VertexTypes.h"
#include "Material.h"
#include "GpuProgram.h"
#include "SceneNode.h"

#include <btBulletDynamicsCommon.h>

typedef struct {
    float top, bottom, left, right, front, back;
} BoundingBox;

class TexturedMesh : public SceneNode
{
public:
    TexturedMesh(const std::string *name,
        const std::string *material,
        std::vector<PosNormTexCoordVertex> *vertexData,
        GLenum primitiveMode,
        glm::mat4 *modelViewMatrix);
    virtual ~TexturedMesh();

    BoundingBox* getBoundingBox();
    void calcBoundingSphere();

    void glInit();
    void draw();

    const std::string getMaterialName() { return material; }
    GLuint *getDiffuseTextureIdPtr() { return &diffuseTextureId; }
    const glm::vec4 getBoundingSphereCenter() { return glm::vec4(lx, ly, lz, 1.f); }
    const GLfloat getBoundingSphereRadius() { return boundingSphere; }
    btCollisionObject *getCollisionObjectPtr() { return collisionObject; }

protected:
    std::string material;
    std::vector<PosNormTexCoordVertex> vertexData;
    std::vector<GLuint> indices;

    GLuint ambientTextureId;
    GLuint diffuseTextureId;
    GLuint normalTextureId;
    GLuint specularTextureId;

    GLfloat boundingSphere;
    GLfloat lx, ly, lz;
    BoundingBox *bbox;

    btCollisionObject *collisionObject;
};