#include "opengl_texture.h"
#include <stb_image.h>

namespace Zafkiel
{
OpenGLTexture2D::OpenGLTexture2D(const TextureSpecification &spec, const Buffer &buffer)
    : width(spec.width), height(spec.height)
{
    switch (spec.format)
    {
        using enum ImageFormat;
    case RGB8:
        internalFormat = GL_RGB8;
        dataFormat = GL_RGB;
        break;
    case RGBA8:
        internalFormat = GL_RGBA8;
        dataFormat = GL_RGBA;
        break;
    default:
        Log::CoreError("Unknown data format!");
        break;
    }

    glCreateTextures(GL_TEXTURE_2D, 1, &rendererID);
    glTextureStorage2D(rendererID, 1, internalFormat, width, height);

    glTextureParameteri(rendererID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(rendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTextureParameteri(rendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(rendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    if (!buffer.empty())
    {
        glTextureSubImage2D(rendererID, 0, 0, 0, width, height, dataFormat, GL_UNSIGNED_BYTE, buffer.data());
    }
}
OpenGLTexture2D::OpenGLTexture2D(const Path &path)
{
    int width, height, channels;
    stbi_set_flip_vertically_on_load(1);
    auto data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if (!data)
    {
        Log::CoreError("Failed to load image!");
    }
    this->width = width;
    this->height = height;

    internalFormat = 0, dataFormat = 0;
    if (channels == 4)
    {
        internalFormat = GL_RGBA8;
        dataFormat = GL_RGBA;
    }
    else if (channels == 3)
    {
        internalFormat = GL_RGB8;
        dataFormat = GL_RGB;
    }
    if (!internalFormat || !dataFormat)
    {
        Log::CoreError("Format not supported!");
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glCreateTextures(GL_TEXTURE_2D, 1, &rendererID);
    glTextureStorage2D(rendererID, 1, internalFormat, width, height);

    glTextureParameteri(rendererID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(rendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTextureParameteri(rendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(rendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTextureSubImage2D(rendererID, 0, 0, 0, width, height, dataFormat, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
}
OpenGLTexture2D::~OpenGLTexture2D()
{
    glDeleteTextures(1, &rendererID);
}
void OpenGLTexture2D::Bind(uint32_t slot) const
{
    glBindTextureUnit(slot, rendererID);
}
void OpenGLTexture2D::SetData(const Buffer &buffer)
{
    int bytesPerPixel = dataFormat == GL_RGBA ? 4 : 3;
    if (buffer.size() != width * height * bytesPerPixel)
    {
        Log::CoreError("Data must be entire texture!");
    }
    glTextureSubImage2D(rendererID, 0, 0, 0, width, height, dataFormat, GL_UNSIGNED_BYTE, buffer.data());
}
}