#pragma once

#include "Common.h"
#include "VertexTypes.h"
#include "Material.h"
#include "GpuProgram.h"
#include "SceneNode.h"
#include "TextureBuffer.h"

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

    const std::string getMaterialName() const { return material; }
    GLuint *getDiffuseTextureIdPtr() { return &diffuseTextureId; }
    const glm::vec4 getBoundingSphereCenter() const { return glm::vec4(lx, ly, lz, 1.f); }
    const GLfloat getBoundingSphereRadius() const { return boundingSphere; }
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

class ScreenQuad : public SceneNode
{
public:
    static ScreenQuad *Create(
        const std::string &name
    )
    {
        std::vector<PosNormTexCoordVertex> vData;
        std::vector<GLuint> iData = { 0, 1, 2, 1, 3, 2 };
        PosNormTexCoordVertex tempVertex;
        tempVertex.position[0] = -1; tempVertex.position[1] = -1; tempVertex.position[2] = 0;
        tempVertex.normal[0] = 0; tempVertex.normal[1] = 0; tempVertex.normal[2] = -1;
        tempVertex.textureCoordinate[0] = 0; tempVertex.textureCoordinate[1] = 0;
        vData.push_back(tempVertex);
        tempVertex.position[0] =  1; tempVertex.position[1] = -1; tempVertex.position[2] = 0;
        tempVertex.normal[0] = 0; tempVertex.normal[1] = 0; tempVertex.normal[2] = -1;
        tempVertex.textureCoordinate[0] = 1; tempVertex.textureCoordinate[1] = 0;
        vData.push_back(tempVertex);
        tempVertex.position[0] = -1; tempVertex.position[1] =  1; tempVertex.position[2] = 0;
        tempVertex.normal[0] = 0; tempVertex.normal[1] = 0; tempVertex.normal[2] = -1;
        tempVertex.textureCoordinate[0] = 0; tempVertex.textureCoordinate[1] = 1;
        vData.push_back(tempVertex);
        tempVertex.position[0] =  1; tempVertex.position[1] =  1; tempVertex.position[2] = 0;
        tempVertex.normal[0] = 0; tempVertex.normal[1] = 0; tempVertex.normal[2] = -1;
        tempVertex.textureCoordinate[0] = 1; tempVertex.textureCoordinate[1] = 1;
        vData.push_back(tempVertex);
        return new ScreenQuad(name, vData, iData);
    }

    void draw()
    {
        bindBuffers();
        glDrawElements(primitiveMode, indices.size(), GL_UNSIGNED_INT, nullptr);
        unbindBuffers();
    }

    ~ScreenQuad()
    {
        vertexData.clear();
        indices.clear();
    }

protected:
    ScreenQuad(
        const std::string &name,
        const std::vector<PosNormTexCoordVertex> &vData,
        const std::vector<GLuint> &iData
    )
        : SceneNode(&name, GL_TRIANGLES)
    {
        vertexData = std::vector<PosNormTexCoordVertex>(vData);
        indices = std::vector<GLuint>(iData);
        glInit();
    }

    void glInit()
    {
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(PosNormTexCoordVertex) * vertexData.size(), &vertexData[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(PosNormTexCoordVertex), (void*)0);                       //send positions on pipe 0
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(PosNormTexCoordVertex), (void*)(sizeof(float) * 6));     //send uvs on pipe 1
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices[0], GL_STATIC_DRAW);
        glBindVertexArray(0);
    }

    std::vector<PosNormTexCoordVertex> vertexData;
    std::vector<GLuint> indices;
};