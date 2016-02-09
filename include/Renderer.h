#ifndef _RENDERER_H_
#define _RENDERER_H_

#include "Camera.h"
#include "Common.h"
#include "Frustum.h"
#include "GpuProgram.h"
#include "Material.h"
#include "SceneNode.h"
#include "Scene.h"
#include "PhysicsInterface.h"

#include <SDL_image.h>

void _checkForGLError(const char *file, int line);
// Usage
// [some opengl calls]
// glCheckError();
#define checkForGLError() _checkForGLError(__FILE__,__LINE__)

class Renderer
{
public:
    Renderer();
    ~Renderer();
    void glInitFromScene(Scene *scene);
    void render(Camera*);
private:
    glm::mat4 modelViewProjectionMatrix;
    GpuProgram *gpuProgram;
    std::vector<Vertex> vertexData;
    Frustum frustum;
    std::vector<GLuint> indices;
    Scene *sceneToBeRendered;
};

#endif
