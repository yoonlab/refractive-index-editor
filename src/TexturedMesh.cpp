#include "TexturedMesh.h"
#include "PhysicsInterface.h"

TexturedMesh::TexturedMesh(const std::string * name,
    const std::string * material,
    std::vector<PosNormTexCoordVertex>* vertexData,
    GLenum primitiveMode,
    glm::mat4 * modelViewMatrix)
    : SceneNode(name, primitiveMode, modelViewMatrix)
{
    bbox = NULL;
    this->material = std::string(*material);
    this->vertexData = std::vector<PosNormTexCoordVertex>(*vertexData);
    this->collisionObject = PhysicsInterface::createCollisionObjectFromVertices(vertexData);
    this->diffuseTextureId = 0;
}

TexturedMesh::~TexturedMesh()
{
    if (bbox != NULL)
    {
        delete bbox;
        bbox = NULL;
    }
    vertexData.clear();
    delete collisionObject->getCollisionShape();
    delete collisionObject;
}

BoundingBox* TexturedMesh::getBoundingBox()
{
    if (bbox != NULL)
        return bbox;
    bbox = new BoundingBox;
    bbox->top = bbox->right = bbox->front = -INFINITY;
    bbox->bottom = bbox->left = bbox->back = INFINITY;
    for (auto position : vertexData)
    {
        bbox->left = glm::min(bbox->left, position.position[0]);
        bbox->right = glm::max(bbox->right, position.position[0]);
        bbox->bottom = glm::min(bbox->bottom, position.position[1]);
        bbox->top = glm::max(bbox->top, position.position[1]);
        bbox->back = glm::min(bbox->back, position.position[2]);
        bbox->front = glm::max(bbox->front, position.position[2]);
    }
    return bbox;
}

void TexturedMesh::calcBoundingSphere()
{
    lx = ly = lz = 0.f;
    boundingSphere = 0.f;
    for (auto position : vertexData)
    {
        lx += position.position[0];
        ly += position.position[1];
        lz += position.position[2];
    }
    lx /= (vertexData.size());
    ly /= (vertexData.size());
    lz /= (vertexData.size());

    for (auto position : vertexData)
    {
        double nx = position.position[0] - lx;
        double ny = position.position[1] - ly;
        double nz = position.position[2] - lz;
        float r2 = sqrt(nx*nx + ny*ny + nz*nz);
        if (r2 > boundingSphere)
        {
            boundingSphere = r2;
        }
    }
    if (boundingSphere == 0.f)
    {
        boundingSphere = 0.1f;
    }
}

void TexturedMesh::glInit()
{
    for (auto position : vertexData)
    {
        indices.push_back(indices.size());
    }
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(PosNormTexCoordVertex) * vertexData.size(), &vertexData[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(PosNormTexCoordVertex), (void*)0);                       //send positions on pipe 0
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(PosNormTexCoordVertex), (void*)(sizeof(float) * 3));       //send normals on pipe 1
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(PosNormTexCoordVertex), (void*)(sizeof(float) * 6));     //send colors on pipe 2
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices[0], GL_STATIC_DRAW);
    glBindVertexArray(0);
}

void TexturedMesh::draw()
{
    bindBuffers();
    glDrawElements(primitiveMode, indices.size(), GL_UNSIGNED_INT, nullptr);
    unbindBuffers();
}