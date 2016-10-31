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
#include "TextureBuffer.h"
#include "Framebuffer.h"
#include "TestModel.h"

#include <SDL_image.h>

void _checkForGLError(const char *file, int line);
// Usage
// [some opengl calls]
// glCheckError();
#define checkForGLError() _checkForGLError(__FILE__,__LINE__)

class Renderer
{
public:
    Renderer(int screenWidth, int screenHeight);
    ~Renderer();
    void glInitFromScene(Scene *scene);
    void render(const Camera &, bool visRefraction, bool visCurve, bool visPoint, TestModel *);
private:
    void shaderInit();
    void textureInit(int texWidth, int texHeight);
    void framebufferinit();

    glm::mat4 modelViewProjectionMatrix;
    GpuProgram *texMeshShaderProgram;
    GpuProgram *ptIndicatorShaderProgram;
    GpuProgram *compositeShaderProgram;
    TextureBuffer *screenColor;
    TextureBuffer *screenDepth;
    TextureBuffer *actualDepth;
    Framebuffer *colorFBO;

    ScreenQuad *quad;

    bool printed;

    Frustum frustum;
    std::vector<GLuint> indices;
    Scene *sceneToBeRendered;

    int screenWidth;
    int screenHeight;
};

#endif
