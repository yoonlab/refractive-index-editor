#pragma once

#include "Common.h"

class TextureBuffer
{
public:
    enum Types
    {
        TYPE_DEPTH,
        TYPE_COLOR
    };

    GLuint getTexHandle() const { return texHandle; }
    Types getType() const { return type; }

    static TextureBuffer *Create(int width, int height,
        GLuint pixelInteralFormat, GLuint pixelFormat, GLuint pixelType,
        GLuint minFilter, GLuint magFilter,
        GLuint wrap)
    {
        GLuint texHandle;
        Types type;
        glGenTextures(1, &texHandle);
        glBindTexture(GL_TEXTURE_2D, texHandle);
        {
            glTexImage2D(GL_TEXTURE_2D, 0, pixelInteralFormat,
                width, height, 0, pixelFormat, pixelType, nullptr);

            if (pixelFormat == GL_DEPTH_COMPONENT)
                type = TYPE_DEPTH;
            else
                type = TYPE_COLOR;

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
        }
        glBindTexture(GL_TEXTURE_2D, 0);
        return new TextureBuffer(texHandle, type);
    }

    ~TextureBuffer()
    {
        if (texHandle != 0)
            glDeleteTextures(1, &texHandle);
        texHandle = 0;
    }

private:
    TextureBuffer(GLuint _texHandle, Types _type)
        : texHandle(_texHandle), type(_type)
    {}

    GLuint texHandle;
    Types type;
};