#include "opengl_image.h"
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
    case RGBA16F: return {GL_RGBA16F, GL_RGBA, GL_FLOAT};
    case RGBA32F: return {GL_RGBA32F, GL_RGBA, GL_FLOAT};
    case R32UI: return {GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT};
    case DEPTH24STENCIL8: return {GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8};
    default:
        Log::Error("Unsupported Image Format!");
        return {};
    }
};

OpenGLImageBackend::OpenGLImageBackend(const ImageSpecification &spec)
    : format(spec.format)
{
	glCreateTextures(GL_TEXTURE_2D, 1, &rendererID);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    OpenGLImageFormat format = ImageFormatToOpenGLType(spec.format);
    glTextureStorage2D(rendererID, 1, format.internalFormat, spec.width, spec.height);
}

OpenGLImageBackend::~OpenGLImageBackend()
{
    glDeleteTextures(1, &rendererID);
}

void OpenGLImageBackend::Resize(uint32_t width, uint32_t height)
{
    glDeleteTextures(1, &rendererID);
	glCreateTextures(GL_TEXTURE_2D, 1, &rendererID);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    OpenGLImageFormat openglFormat = ImageFormatToOpenGLType(format);
    glTextureStorage2D(rendererID, 1, openglFormat.internalFormat, width, height);
}

Scope<Image> OpenGLImageFactory::Create(const ImageSpecification &spec)
{
    auto backend = CreateScope<OpenGLImageBackend>(spec);
    return CreateScope<Image>(spec, std::move(backend));
}

}
