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
    vao = vbo = ibo = 0;
    gpuProgram = 0;
	sceneToBeRendered = NULL;
}

Renderer::~Renderer()
{
    if(sceneToBeRendered != NULL && sceneToBeRendered->sceneNodes.size() > 0)
    {
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ibo);
        glDeleteVertexArrays(1, &vao);
    }
}

// Used for debugging
void printVertex(Vertex& v)
{
    std::cerr << "v " << v.vertex[0] << ", " << v.vertex[1] << ", " << v.vertex[2]
            << '\t' << " n " << v.normal[0] << ", " << v.normal[1] << ", " << v.normal[2]
            << '\t' << " t " << v.textureCoordinate[0] << ", " << v.textureCoordinate[1] << std::endl;
}

void Renderer::glInitFromScene(Scene *scene)
{
    checkForGLError();
    if(scene->sceneNodes.size() == 0)
    {
        std::cout << "building empty scene" << std::endl;
        return;
    }

	scene->prepare();

	sceneToBeRendered = scene;

    for(int i=0; i<scene->sceneNodes.size(); i++)
    {
        for(int j=0; j<scene->sceneNodes[i].vertexDataSize; j++)
        {
            Vertex v;
            memcpy((void*) &v, (void*) &(scene->sceneNodes[i].vertexData[j]), sizeof(Vertex));
            vertexData.push_back(v);
            indices.push_back(indices.size());
        }
    }

    //std::cout << "num scene nodes: " << sceneNodes.size() << std::endl;
    //std::cout << "num vertices: " << vertexData.size() << std::endl;
    //std::cout << "num indices: " << indices.size() << std::endl;

    checkForGLError();

    //Allocate and assign a Vertex Array Object to our handle
    glGenVertexArrays(1, &vao);
    checkForGLError();
    // Bind our Vertex Array Object as the current used object
    glBindVertexArray(vao);
    checkForGLError();

    //Triangle Vertices
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexData.size(), &vertexData[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)0);                       //send positions on pipe 0
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)(sizeof(float)*3));       //send normals on pipe 1
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)(sizeof(float)*6));     //send colors on pipe 2
    //glBindBuffer(GL_ARRAY_BUFFER, 0);

    checkForGLError();

    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices[0], GL_STATIC_DRAW);
    checkForGLError();
    gpuProgram = new GpuProgram();

    try
    {

        VertexShader vertShader("shaders/default.vert");
        FragmentShader fragShader("shaders/default.frag");

        gpuProgram->attachShader(vertShader);
        gpuProgram->attachShader(fragShader);

        glLinkProgram(gpuProgram->getId());

        // Check for link errors
        GLint result;
        GLuint programId = gpuProgram->getId();
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
    if(sceneToBeRendered->sceneNodes.size() == 0)
    {
        std::cout << "skipping render() on empty scene" << std::endl;
        return;
    }
#if _DEBUG
    checkForGLError();
#endif
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

#if _DEBUG
    checkForGLError();
#endif

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

    glm::vec3 lightPos = glm::vec3(10,135,0);
    //TODO: move uniforms to buffer object
    GLuint programID = gpuProgram->getId();
    // Get a handle for our "MVP" uniform
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
    GLuint ModelMatrixID = glGetUniformLocation(programID, "M");
    GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
    GLuint ambientLocation = glGetUniformLocation(programID, "MaterialAmbient");
    GLuint diffuseLocation = glGetUniformLocation(programID, "MaterialDiffuse");
    GLuint specularLocation = glGetUniformLocation(programID, "MaterialSpecular");

    frustum.extractFrustum(camera->modelViewMatrix, camera->projectionMatrix);
    for(int i=0; i<sceneToBeRendered->sceneNodes.size(); i++)
    {
        glm::vec4 position(sceneToBeRendered->sceneNodes[i].lx, sceneToBeRendered->sceneNodes[i].ly, sceneToBeRendered->sceneNodes[i].lz, 1.f);

        // Frustum culling test
        if(frustum.spherePartiallyInFrustum(position.x, position.y, position.z, sceneToBeRendered->sceneNodes[i].boundingSphere) > 0)
        {
            // create and upload modelviewprojection matrix
            modelViewProjectionMatrix = camera->projectionMatrix * (camera->modelViewMatrix * sceneToBeRendered->sceneNodes[i].modelViewMatrix) ;

#if _DEBUG
            checkForGLError();
#endif

            glActiveTexture(GL_TEXTURE0);
#if _DEBUG
            checkForGLError();
#endif
            glBindTexture(GL_TEXTURE_2D, sceneToBeRendered->sceneNodes[i].diffuseTextureId );
#if _DEBUG
            checkForGLError();
#endif



#if _DEBUG
            checkForGLError();
#endif
            gpuProgram->use();
#if _DEBUG
            checkForGLError();
#endif
            glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &modelViewProjectionMatrix[0][0]);
            glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &camera->projectionMatrix[0][0]);
            glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &camera->modelViewMatrix[0][0]);
#if _DEBUG
            checkForGLError();
#endif
            glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
            //std::cout << "setting diffuse color: " << materials[sceneNodes[i].material].diffuse[0] << ", " << materials[sceneNodes[i].material].diffuse[1] << ", " << materials[sceneNodes[i].material].diffuse[2] << std::endl;
            glUniform3fv(ambientLocation, 1, sceneToBeRendered->materials[sceneToBeRendered->sceneNodes[i].material].ambient);
            glUniform3fv(diffuseLocation, 1, sceneToBeRendered->materials[sceneToBeRendered->sceneNodes[i].material].diffuse);
            glUniform3fv(specularLocation, 1, sceneToBeRendered->materials[sceneToBeRendered->sceneNodes[i].material].specular);

            glUniform1i(glGetUniformLocation(gpuProgram->getId(), "myTextureSampler"), 0);
#if _DEBUG
            checkForGLError();
#endif

            glDrawRangeElementsBaseVertex(sceneToBeRendered->sceneNodes[i].primitiveMode, sceneToBeRendered->sceneNodes[i].startPosition, sceneToBeRendered->sceneNodes[i].endPosition,
                    (sceneToBeRendered->sceneNodes[i].endPosition - sceneToBeRendered->sceneNodes[i].startPosition), GL_UNSIGNED_INT, (void*)(0), sceneToBeRendered->sceneNodes[i].startPosition);
        }
        else
        {
            //std::cerr << "Clipping " << sceneNodes[i].name << std::endl;
        }
#if _DEBUG
        checkForGLError();
#endif
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glBindVertexArray(0);

#if _DEBUG
    checkForGLError();
#endif
}
