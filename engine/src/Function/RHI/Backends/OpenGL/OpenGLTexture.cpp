#include "Function/RHI/Backends/OpenGL/OpenGLTexture.h"
#include "Function/RHI/Backends/OpenGL/OpenGLRHI.h"
#include <glad/glad.h>

namespace Zafkiel
{
OpenGLImageFormat ImageFormatToOpenGLType(ImageFormat format)
{
    switch (format)
    {
        using enum ImageFormat;
    case R8: return {GL_R8, GL_RED, GL_UNSIGNED_BYTE};
    case RG8: return {GL_RG8, GL_RG, GL_UNSIGNED_BYTE};
    case RGB8: return {GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE};
    case RGBA8: return {GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE};
    case BGR8: return {GL_RGB8, GL_BGR, GL_UNSIGNED_BYTE};
    case BGRA8: return {GL_RGBA8, GL_BGRA, GL_UNSIGNED_BYTE};
    case R8_sRGB: return {GL_SRGB8, GL_RED, GL_UNSIGNED_BYTE};
    case RG8_sRGB: return {GL_SRGB8, GL_RG, GL_UNSIGNED_BYTE};
    case RGB8_sRGB: return {GL_SRGB8, GL_RGB, GL_UNSIGNED_BYTE};
    case RGBA8_sRGB: return {GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_BYTE};
    case BGR8_sRGB: return {GL_SRGB8, GL_BGR, GL_UNSIGNED_BYTE};
    case BGRA8_sRGB: return {GL_SRGB8_ALPHA8, GL_BGRA, GL_UNSIGNED_BYTE};
    case RGB16F: return {GL_RGB16F, GL_RGB, GL_FLOAT};
    case RGB32F: return {GL_RGB32F, GL_RGB, GL_FLOAT};
    case RGBA16F: return {GL_RGBA16F, GL_RGBA, GL_FLOAT};
    case RGBA32F: return {GL_RGBA32F, GL_RGBA, GL_FLOAT};
    case R32UI: return {GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT};
    case R32F: return {GL_R32F, GL_RED, GL_FLOAT};
    case DEPTH24STENCIL8: return {GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8};
    case DEPTH32F: return {GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT}; // TODO: 确认正确
    default:
        Log::Error("Unsupported Image Format!");
        return {};
    }
};

GLenum TextureWrapToOpenGLType(TextureWrap wrap)
{
    switch (wrap)
    {
        using enum TextureWrap;
    case Clamp: return GL_CLAMP_TO_EDGE;
    case Repeat: return GL_REPEAT;
    default:
        Log::Error("Unsupported TextureWrap!");
        return GL_REPEAT;
    }
}
GLenum FilterTypeToOpenGLType(TextureFilter filter)
{
    switch (filter)
    {
        using enum TextureFilter;
    case Nearest: return GL_NEAREST;
    case Linear: return GL_LINEAR;
    default:
        Log::Error("Unsupported TextureFilter!");
        return GL_NEAREST;
    }
}

OpenGLTexture::OpenGLTexture(OpenGLRHI &rhi, const RHITextureDesc &desc, Buffer data)
    : rhi(rhi), RHITexture(desc)
{
	glCreateTextures(GL_TEXTURE_2D, 1, &handle);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    OpenGLImageFormat format = ImageFormatToOpenGLType(desc.format);
    glTextureStorage2D(handle, 1, format.internalFormat, desc.width, desc.height);

    GLenum wrap = TextureWrapToOpenGLType(desc.wrap);
    GLenum filter = FilterTypeToOpenGLType(desc.filter);

    glTextureParameteri(handle, GL_TEXTURE_MIN_FILTER, filter);
    glTextureParameteri(handle, GL_TEXTURE_MAG_FILTER, filter);

    glTextureParameteri(handle, GL_TEXTURE_WRAP_S, wrap);
    glTextureParameteri(handle, GL_TEXTURE_WRAP_T, wrap);

    if (data)
    {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTextureSubImage2D(handle, 0, 0, 0,
            desc.width, desc.height, format.dataFormat, format.dataType, data.Data<void>());
    }
}

OpenGLTexture::~OpenGLTexture()
{
    rhi.GetFrameBufferManager().OnDestroyTexture(this);
    glDeleteTextures(1, &handle);
}

}
