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

Renderer::Renderer()
{
    texMeshShaderProgram = NULL;
    ptIndicatorShaderProgram = NULL;
    sceneToBeRendered = NULL;
}

Renderer::~Renderer()
{
    if (texMeshShaderProgram != NULL) delete texMeshShaderProgram; texMeshShaderProgram = NULL;
    if (ptIndicatorShaderProgram != NULL) delete ptIndicatorShaderProgram; ptIndicatorShaderProgram = NULL;
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

    texMeshShaderProgram = new GpuProgram();

    try
    {
        VertexShader vertShader("shaders/default.vert");
        FragmentShader fragShader("shaders/default.frag");

        texMeshShaderProgram->attachShader(vertShader);
        texMeshShaderProgram->attachShader(fragShader);

        glLinkProgram(texMeshShaderProgram->getId());

        // Check for link errors
        GLint result;
        GLuint programId = texMeshShaderProgram->getId();
        glGetProgramiv(programId, GL_LINK_STATUS, &result);
        if(result == GL_FALSE)
        {
            GLint length;
            char *log;
            // get the program info log
            glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &length);
            log = (char*) malloc(length);
            glGetProgramInfoLog(programId, length, &result, log);
            std::cout << "Unable to link shader: " << log << std::endl;
            free(log);
        }
        checkForGLError();
    }

    catch (std::exception& e)
    {
        std::cerr << "Unable to load shader: " << e.what() << std::endl;
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    checkForGLError();
}

void Renderer::render(Camera* camera)
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

    frustum.extractFrustum(camera->modelViewMatrix, camera->projectionMatrix);
    for (auto texMesh : sceneToBeRendered->texturedMeshes)
    {
        const glm::vec4 position = texMesh->getBoundingSphereCenter();
        if (frustum.spherePartiallyInFrustum(position.x, position.y, position.z, texMesh->getBoundingSphereRadius()) > 0)
        {
            modelViewProjectionMatrix = camera->projectionMatrix * (camera->modelViewMatrix * texMesh->getModelViewMatrix());

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, *texMesh->getDiffuseTextureIdPtr());

            texMeshShaderProgram->use();

            glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &modelViewProjectionMatrix[0][0]);
            glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &camera->projectionMatrix[0][0]);
            glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &camera->modelViewMatrix[0][0]);

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
}
