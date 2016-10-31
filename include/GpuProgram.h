#ifndef _GPU_PROGRAM_H_
#define _GPU_PROGRAM_H_

#include "Common.h"
#include "Shader.h"

class GpuProgram
{
public:
    GpuProgram(std::string &vSrc, std::string &fSrc);
    GpuProgram(std::string &vSrc, std::string &fSrc, std::string &gSrc);
    ~GpuProgram();
    GLuint getId();
    void use();
private:
    void checkError();
    GLuint id;
};

#endif