#include "GpuProgram.h"

GpuProgram::GpuProgram(std::string &vSrc, std::string &fSrc)
{
    try
    {
        VertexShader vertShader(vSrc);
        FragmentShader fragShader(fSrc);
        id = glCreateProgram();
        glAttachShader(id, vertShader.getId());
        glAttachShader(id, fragShader.getId());
        glBindFragDataLocation(id, 0, "color");
        glBindFragDataLocation(id, 1, "depth");
        glLinkProgram(id);
    }
    catch (std::exception &e)
    {
        throw e;
    }
}

GpuProgram::GpuProgram(std::string &vSrc, std::string &fSrc, std::string &gSrc)
{
    try
    {
        VertexShader vertShader(vSrc);
        FragmentShader fragShader(fSrc);
        GeometryShader geomShader(gSrc);
        id = glCreateProgram();
        glAttachShader(id, vertShader.getId());
        glAttachShader(id, fragShader.getId());
        glAttachShader(id, geomShader.getId());
        glBindFragDataLocation(id, 0, "color");
        glBindFragDataLocation(id, 1, "depth");
        glLinkProgram(id);
    }
    catch (std::exception &e)
    {
        throw e;
    }
}

GpuProgram::~GpuProgram()
{
    glDeleteProgram(id);
}

GLuint GpuProgram::getId()
{
    return id;
}

void GpuProgram::use()
{
    glUseProgram(id);
}

void GpuProgram::checkError()
{
    GLint result;
    GLuint programId = id;
    glGetProgramiv(programId, GL_LINK_STATUS, &result);
    if (result == GL_FALSE)
    {
        GLint length;
        char *log;
        // get the program info log
        glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &length);
        log = (char*)malloc(length);
        glGetProgramInfoLog(programId, length, &result, log);
        std::cout << "Unable to link shader: " << log << std::endl;
        free(log);
    }
}
