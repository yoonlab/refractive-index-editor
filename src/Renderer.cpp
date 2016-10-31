#include "Common.h"
#include "Renderer.h"

void _checkForGLError(const char *file, int line)
{
    GLenum err (glGetError());
    static int numErrors = 0;
    while(err!=GL_NO_ERROR)
    {
        std::string error;
        numErrors++;
        switch(err)
        {
        case GL_INVALID_OPERATION:
            error="INVALID_OPERATION";
            break;
        case GL_INVALID_ENUM:
            error="INVALID_ENUM";
            break;
        case GL_INVALID_VALUE:
            error="INVALID_VALUE";
            break;
        case GL_OUT_OF_MEMORY:
            error="OUT_OF_MEMORY";
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            error="INVALID_FRAMEBUFFER_OPERATION";
            break;
        }

        std::cerr << "GL_" << error.c_str() <<" - "<< file << ":" << line << std::endl;

        if(numErrors > 10)
        {
            exit(1);
        }

        err=glGetError();
    }
}

Renderer::Renderer(int _screenWidth, int _screenHeight)
    : screenWidth(_screenWidth), screenHeight(_screenHeight)
{
    texMeshShaderProgram = NULL;
    ptIndicatorShaderProgram = NULL;
    compositeShaderProgram = nullptr;
    screenColor = nullptr;
    screenDepth = nullptr;
    actualDepth = nullptr;
    colorFBO = nullptr;
    quad = nullptr;
    sceneToBeRendered = NULL;
    printed = false;
}

Renderer::~Renderer()
{
    if (texMeshShaderProgram != NULL) delete texMeshShaderProgram; texMeshShaderProgram = NULL;
    if (ptIndicatorShaderProgram != NULL) delete ptIndicatorShaderProgram; ptIndicatorShaderProgram = NULL;
    if (compositeShaderProgram != nullptr) delete compositeShaderProgram; compositeShaderProgram = nullptr;
    if (screenColor != nullptr) delete screenColor; screenColor = nullptr;
    if (screenDepth != nullptr) delete screenDepth; screenDepth = nullptr;
    if (actualDepth != nullptr) delete actualDepth; actualDepth = nullptr;
    if (colorFBO != nullptr) delete colorFBO; colorFBO = nullptr;
    if (quad != nullptr) delete quad; quad = nullptr;
}

// Used for debugging
void printVertex(PosNormTexCoordVertex& v)
{
    std::cerr << "v " << v.position[0] << ", " << v.position[1] << ", " << v.position[2]
            << '\t' << " n " << v.normal[0] << ", " << v.normal[1] << ", " << v.normal[2]
            << '\t' << " t " << v.textureCoordinate[0] << ", " << v.textureCoordinate[1] << std::endl;
}

void Renderer::glInitFromScene(Scene *scene)
{
    checkForGLError();
    if(scene->texturedMeshes.size() == 0)
    {
        std::cout << "building empty scene" << std::endl;
        return;
    }

    scene->prepare();
    assert(scene->getIsPrepared());

    sceneToBeRendered = scene;
    shaderInit();
    textureInit(screenWidth, screenHeight);
    framebufferinit();

    quad = ScreenQuad::Create(std::string("Screen-filling quad"));

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    checkForGLError();
}

void Renderer::shaderInit()
{
    try
    {
        texMeshShaderProgram = new GpuProgram(
            std::string("shaders/default.vert"), 
            std::string("shaders/default.frag")
        );
        checkForGLError();
    }
    catch (std::exception& e)
    {
        std::cerr << "Unable to load shader: " << e.what() << std::endl;
    }

    try
    {
        ptIndicatorShaderProgram = new GpuProgram(
            std::string("shaders/point.vert"),
            std::string("shaders/point.frag"),
            std::string("shaders/point.geom")
        );
        checkForGLError();
    }
    catch (std::exception& e)
    {
        std::cerr << "Unable to load shader: " << e.what() << std::endl;
    }

    try
    {
        compositeShaderProgram = new GpuProgram(
            std::string("shaders/composite.vert"),
            std::string("shaders/composite.frag")
        );
        checkForGLError();
    }
    catch (std::exception& e)
    {
        std::cerr << "Unable to load shader: " << e.what() << std::endl;
    }
}

void Renderer::textureInit(int texWidth, int texHeight)
{
    if (screenColor == nullptr) screenColor = TextureBuffer::Create(texWidth, texHeight, GL_RGBA16F, GL_RGBA, GL_FLOAT, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE);
    if (screenDepth == nullptr) screenDepth = TextureBuffer::Create(texWidth, texHeight, GL_R32F, GL_RED, GL_UNSIGNED_INT, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE);
    if (actualDepth == nullptr) actualDepth = TextureBuffer::Create(texWidth, texHeight, GL_DEPTH32F_STENCIL8, GL_DEPTH_COMPONENT, GL_FLOAT, GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_BORDER);
}

void Renderer::framebufferinit()
{
    if (colorFBO == nullptr) colorFBO = Framebuffer::Create({ screenColor, screenDepth, actualDepth });
}

void Renderer::render(const Camera &camera, bool visRefraction, bool visCurve, bool visPoint, TestModel *model)
{
    if(sceneToBeRendered->texturedMeshes.size() == 0)
    {
        std::cout << "skipping render() on empty scene" << std::endl;
        return;
    }
    assert(sceneToBeRendered->getIsPrepared());

    glm::vec3 lightPos = glm::vec3(10,135,0);
    //TODO: move uniforms to buffer object
    GLuint programID = texMeshShaderProgram->getId();
    // Get a handle for our "MVP" uniform
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
    GLuint ModelMatrixID = glGetUniformLocation(programID, "M");
    GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
    GLuint ambientLocation = glGetUniformLocation(programID, "MaterialAmbient");
    GLuint diffuseLocation = glGetUniformLocation(programID, "MaterialDiffuse");
    GLuint specularLocation = glGetUniformLocation(programID, "MaterialSpecular");

    frustum.extractFrustum(camera.modelViewMatrix, camera.projectionMatrix);
    glEnable(GL_DEPTH_TEST);

    colorFBO->Bind();
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_DEPTH_BUFFER_BIT);
    glClearColor(0.8, 0.8, 0.8, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    for (const auto &texMesh : sceneToBeRendered->texturedMeshes)
    {
        const glm::vec4 position = texMesh->getBoundingSphereCenter();
        if (frustum.spherePartiallyInFrustum(position.x, position.y, position.z, texMesh->getBoundingSphereRadius()) > 0)
        {
            modelViewProjectionMatrix = camera.projectionMatrix * (camera.modelViewMatrix * texMesh->getModelViewMatrix());

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, *texMesh->getDiffuseTextureIdPtr());

            texMeshShaderProgram->use();

            glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &modelViewProjectionMatrix[0][0]);
            glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &camera.projectionMatrix[0][0]);
            glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &camera.modelViewMatrix[0][0]);

            glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
            glUniform3fv(ambientLocation, 1, sceneToBeRendered->materials[texMesh->getMaterialName()].ambient);
            glUniform3fv(diffuseLocation, 1, sceneToBeRendered->materials[texMesh->getMaterialName()].diffuse);
            glUniform3fv(specularLocation, 1, sceneToBeRendered->materials[texMesh->getMaterialName()].specular);

            glUniform1i(glGetUniformLocation(texMeshShaderProgram->getId(), "myTextureSampler"), 0);
            texMesh->draw();
        }
        else
        {
            //std::cerr << "Clipping " << texMesh.name << std::endl;
        }
    }

    if (!visRefraction)
    {
        if (sceneToBeRendered->pointIndicator != NULL && visPoint)
        {
            glDisable(GL_DEPTH_TEST);
            programID = ptIndicatorShaderProgram->getId();
            GLuint ProjectionMatrixID = glGetUniformLocation(programID, "ProjectionMatrix");
            ViewMatrixID = glGetUniformLocation(programID, "ViewMatrix");
            ptIndicatorShaderProgram->use();
            glUniformMatrix4fv(ProjectionMatrixID, 1, GL_FALSE, &camera.projectionMatrix[0][0]);
            glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &camera.modelViewMatrix[0][0]);
            sceneToBeRendered->pointIndicator->draw();
        }

        if (visCurve)
        {
            for (const auto &pCurve : sceneToBeRendered->curves)
            {
                // To be updated
                glEnable(GL_DEPTH_TEST);
                programID = ptIndicatorShaderProgram->getId();
                GLuint ProjectionMatrixID = glGetUniformLocation(programID, "ProjectionMatrix");
                ViewMatrixID = glGetUniformLocation(programID, "ViewMatrix");
                ptIndicatorShaderProgram->use();
                glUniformMatrix4fv(ProjectionMatrixID, 1, GL_FALSE, &camera.projectionMatrix[0][0]);
                glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &camera.modelViewMatrix[0][0]);
                pCurve->draw();
            }
        }
    }

    colorFBO->Unbind();
    //glClearColor(0.0, 0.0, 0.0, 0.0);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    programID = compositeShaderProgram->getId();
    GLuint depthTexID = glGetUniformLocation(programID, "depthTex");
    GLuint colorTexID = glGetUniformLocation(programID, "colorTex");
    ViewMatrixID = glGetUniformLocation(programID, "ViewMatrix");
    GLuint ProjMatrixID = glGetUniformLocation(programID, "ProjectionMatrix");
    GLuint camPosID = glGetUniformLocation(programID, "camPos");
    GLuint refractiveModeID = glGetUniformLocation(programID, "refractionMode");
    compositeShaderProgram->use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, screenDepth->getTexHandle());
    glUniform1i(depthTexID, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, screenColor->getTexHandle());
    glUniform1i(colorTexID, 1);
    glUniformMatrix4fv(ProjMatrixID, 1, GL_FALSE, &camera.projectionMatrix[0][0]);
    glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &camera.modelViewMatrix[0][0]);;
    glUniform3f(camPosID, camera.position.x, camera.position.y, camera.position.z);
    glUniform1i(refractiveModeID, visRefraction ? 1 : 0);
    GLuint paramsID[8];
    paramsID[0] = glGetUniformLocation(programID, "bgN");
    paramsID[1] = glGetUniformLocation(programID, "bgF");
    paramsID[2] = glGetUniformLocation(programID, "invN");
    paramsID[3] = glGetUniformLocation(programID, "invA");
    paramsID[4] = glGetUniformLocation(programID, "invH");
    paramsID[5] = glGetUniformLocation(programID, "hotN");
    paramsID[6] = glGetUniformLocation(programID, "hotF");
    paramsID[7] = glGetUniformLocation(programID, "scale");
    for (int i = 0; i < 8; ++i)
    {
        /*
        if (!printed && visRefraction)
        {
            std::cout << "param[" << i << "] = " << *model->arguments()->at(i) << std::endl;
        }*/
        glUniform1f(paramsID[i], *model->arguments()->at(i));
    }
    if (!printed && visRefraction) printed = true;
    GLuint nearID = glGetUniformLocation(programID, "near");
    glUniform1f(nearID, camera.near);
    quad->draw();
}
