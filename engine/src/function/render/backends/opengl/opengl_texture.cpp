#include "opengl_texture.h"
#include <stb_image.h>

namespace Zafkiel
{
OpenGLTexture2D::OpenGLTexture2D(const TextureSpecification &spec)
    : width(spec.width), height(spec.height), format(spec.format)
{
    switch (spec.format)
    {
        using enum TextureFormat;
    case RGB8:
        internalFormat = GL_RGB8;
        dataFormat = GL_RGB;
        dataType = GL_UNSIGNED_BYTE;
        bytesPerPixel = 3;
        break;
    case RGBA8:
        internalFormat = GL_RGBA8;
        dataFormat = GL_RGBA;
        dataType = GL_UNSIGNED_BYTE;
        bytesPerPixel = 4;
        break;
    case DEPTH24STENCIL8:
        internalFormat = GL_DEPTH24_STENCIL8;
        dataFormat = GL_DEPTH_STENCIL;
        dataType = GL_UNSIGNED_INT_24_8;
        bytesPerPixel = 4;
        break;
    case R32UI:
        internalFormat = GL_R32UI;
        dataFormat = GL_RED_INTEGER;
        dataType = GL_UNSIGNED_INT;
        bytesPerPixel = 4;
        break;
    default:
        Log::CoreError("Unknown data format!");
        break;
    }
    if (width == 0 || height == 0)
    {
        Log::CoreError("Invalid texture dimensions: {}x{}", width, height);
        return;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glCreateTextures(GL_TEXTURE_2D, 1, &rendererID);
    glTextureStorage2D(rendererID, 1, internalFormat, width, height);

    switch (format)
    {
        using enum TextureFormat;
    case R32UI:
    case DEPTH24STENCIL8:
        glTextureParameteri(rendererID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(rendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        break;
    case RGB8:
    case RGBA8:
        glTextureParameteri(rendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(rendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        break;
    default:
        Log::CoreError("Unknown data format!");
        break;
    }

    glTextureParameteri(rendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(rendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

OpenGLTexture2D::OpenGLTexture2D(const TextureSpecification &spec, const Buffer &buffer)
    : OpenGLTexture2D(spec)
{
    if (!buffer.empty())
    {
        SetData(buffer);
    }
}

OpenGLTexture2D::OpenGLTexture2D(const Path &path)
{
    int width, height, channels;
    stbi_set_flip_vertically_on_load(1);
    auto data = stbi_load(path.string().c_str(), &width, &height, &channels, 0);
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
    dataType = GL_UNSIGNED_BYTE;
    if (!internalFormat || !dataFormat)
    {
        Log::CoreError("Format not supported!");
        stbi_image_free(data);
        return;
    }

    if (width == 0 || height == 0)
    {
        Log::CoreError("Invalid image dimensions: {}x{}", width, height);
        stbi_image_free(data);
        return;
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
    if (format == TextureFormat::None)
    {
        Log::CoreError("Texture format not initialized!");
        return;
    }
    if (width == 0 || height == 0)
    {
        Log::CoreError("Texture dimensions not initialized!");
        return;
    }
    if (buffer.size() != width * height * bytesPerPixel)
    {
        Log::CoreError("Data must be entire texture! Expected: {}, Got: {}", width * height * bytesPerPixel, buffer.size());
        return;
    }
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTextureSubImage2D(rendererID, 0, 0, 0, width, height, dataFormat, dataType, buffer.data());
}

void OpenGLTexture2D::Clear(const void *value)
{
    glClearTexImage(rendererID, 0, dataFormat, dataType, value);
}
}