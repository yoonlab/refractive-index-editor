#ifndef _SCENE_NODE_H_
#define _SCENE_NODE_H_

#include "Common.h"
#include "Material.h"
#include "GpuProgram.h"

#include <btBulletDynamicsCommon.h>

typedef struct {
    float top, bottom, left, right, front, back;
} BoundingBox;

typedef struct {
    std::string name;
    std::string material;
	Vertex* vertexData;
	size_t vertexDataSize;
    glm::mat4 modelViewMatrix;
    GLuint startPosition;
    GLuint endPosition;
    GLenum primitiveMode;

    GLuint ambientTextureId;
    GLuint diffuseTextureId;
    GLuint normalTextureId;
    GLuint specularTextureId;

    GLfloat boundingSphere;
    GLfloat lx, ly, lz;

	btCollisionObject *collisionObject;
} SceneNode;

BoundingBox* getBoundingBox(SceneNode*);

#endif
