#pragma once

#include "Common.h"
#include "TextureBuffer.h"

class Framebuffer
{
public:
    static Framebuffer *Create(std::vector<TextureBuffer *> textures)
    {
        GLuint cTexCount = 0;
        GLuint fbo;
        glGenFramebuffers(1, &fbo);

        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        {
            for (const auto &tex : textures)
            {
                switch (tex->getType())
                {
                case TextureBuffer::Types::TYPE_COLOR:
                    glFramebufferTexture2D(
                        GL_FRAMEBUFFER,
                        GL_COLOR_ATTACHMENT0 + cTexCount,
                        GL_TEXTURE_2D,
                        tex->getTexHandle(),
                        0
                    );
                    cTexCount++;
                    break;
                case TextureBuffer::Types::TYPE_DEPTH:
                    glFramebufferTexture2D(
                        GL_FRAMEBUFFER,
                        GL_DEPTH_ATTACHMENT,
                        GL_TEXTURE_2D,
                        tex->getTexHandle(),
                        0
                    );
                }
            }
            GLenum *drawBuffers = new GLenum[cTexCount];
            for (GLuint drawBufferIter = 0; drawBufferIter < cTexCount; ++drawBufferIter)
            {
                drawBuffers[drawBufferIter] = GL_COLOR_ATTACHMENT0 + drawBufferIter;
            }
            glDrawBuffers(cTexCount, drawBuffers);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        return new Framebuffer(fbo, cTexCount);
    }

    ~Framebuffer()
    {
        if (fbo != 0)
            glDeleteFramebuffers(1, &fbo);
        fbo = 0;
    }

    void Bind() { glBindFramebuffer(GL_FRAMEBUFFER, fbo); }
    void Unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
    GLuint getColorAttachmentCount() const { return cTexCount; }

private:
    Framebuffer(GLuint _fbo, GLuint _cTexCount) : 
        fbo(_fbo), cTexCount(_cTexCount)
    {}

    GLuint fbo;
    GLuint cTexCount;
};