#include "opengl_texture.h"
#include "opengl_image.h"
#include <glad/glad.h>

namespace Zafkiel
{

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

OpenGLTexture2DBackend::OpenGLTexture2DBackend(const Texture2DSpecification &spec)
{
    
}

void OpenGLTexture2DBackend::SetData(Observer<Image> image, Buffer buffer)
{
    OpenGLImageFormat format = ImageFormatToOpenGLType(image->GetFormat());
    uint32_t bytes = ImageFormatToBytes(image->GetFormat());
    if (buffer.Size<uint8_t>() != image->GetWidth() * image->GetHeight() * bytes)
    {
        Log::Error("Data must be entire texture! Expected: {}, Got: {}", image->GetWidth() * image->GetHeight() * bytes, buffer.Size<uint8_t>());
        return;
    }
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTextureSubImage2D(image->GetBackend().As<OpenGLImageBackend>()->GetRendererID(), 0, 0, 0,
        image->GetWidth(), image->GetHeight(), format.dataFormat, format.dataType, buffer.Data<uint8_t>());
}

Scope<Texture2D> OpenGLTexture2DFactory::Create(const Texture2DSpecification &spec)
{
    auto backend = CreateScope<OpenGLTexture2DBackend>(spec);

    ImageSpecification imageSpec
    {
        .format = spec.format,
        .usages = spec.usages,
        .updateFrequency = spec.updateFrequency,
        .width = spec.width,
        .height = spec.height,
        .samples = spec.samples
    };
    auto texture2D = CreateScope<Texture2D>(spec, std::move(backend));
    AccessImage(texture2D) = OpenGLImageFactory::Create(imageSpec);
    
    OpenGLImageFormat openglFormat = ImageFormatToOpenGLType(spec.format);
    GLenum wrap = TextureWrapToOpenGLType(spec.wrap);
    GLenum filter = FilterTypeToOpenGLType(spec.filter);
    uint32_t imageID = texture2D->GetImage()->GetBackend().As<OpenGLImageBackend>()->GetRendererID();
    
    glBindTexture(GL_TEXTURE_2D, imageID);
    glTextureParameteri(imageID, GL_TEXTURE_MIN_FILTER, filter);
    glTextureParameteri(imageID, GL_TEXTURE_MAG_FILTER, filter);

    glTextureParameteri(imageID, GL_TEXTURE_WRAP_S, wrap);
    glTextureParameteri(imageID, GL_TEXTURE_WRAP_T, wrap);
    
    return texture2D;
}

Scope<Texture2D> OpenGLTexture2DFactory::Create(const Texture2DSpecification &spec, Buffer buffer)
{
    auto backend = CreateScope<OpenGLTexture2DBackend>(spec);

    ImageSpecification imageSpec
    {
        .format = spec.format,
        .usages = spec.usages,
        .updateFrequency = spec.updateFrequency,
        .width = spec.width,
        .height = spec.height,
        .samples = spec.samples
    };
    auto texture2D = CreateScope<Texture2D>(spec, std::move(backend));
    if (std::find(imageSpec.usages.begin(), imageSpec.usages.end(), ImageUsage::Upload) == imageSpec.usages.end())
    {
        imageSpec.usages.push_back(ImageUsage::Upload);
    }
    AccessImage(texture2D) = OpenGLImageFactory::Create(imageSpec);
    
    OpenGLImageFormat openglFormat = ImageFormatToOpenGLType(spec.format);
    GLenum wrap = TextureWrapToOpenGLType(spec.wrap);
    GLenum filter = FilterTypeToOpenGLType(spec.filter);
    uint32_t imageID = texture2D->GetImage()->GetBackend().As<OpenGLImageBackend>()->GetRendererID();

    glBindTexture(GL_TEXTURE_2D, imageID);
    glTextureParameteri(imageID, GL_TEXTURE_MIN_FILTER, filter);
    glTextureParameteri(imageID, GL_TEXTURE_MAG_FILTER, filter);

    glTextureParameteri(imageID, GL_TEXTURE_WRAP_S, wrap);
    glTextureParameteri(imageID, GL_TEXTURE_WRAP_T, wrap);
    
    texture2D->GetBackend().As<OpenGLTexture2DBackend>()->SetData(texture2D->GetImage(), buffer);

    return texture2D;
}

// OpenGLCubeMap::OpenGLCubeMap(const std::vector<Path> &paths)
// {
//     glGenTextures(1, &rendererID);
//     glBindTexture(GL_TEXTURE_CUBE_MAP, rendererID);
//     int width, height, nrChannels;
//     unsigned char *data;
//
//     glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
//     for (unsigned int i = 0; i < paths.size(); i++)
//     {
//         data = stbi_load(paths[i].c_str(), &width, &height, &nrChannels, 0);
//
//         if (!data)
//         {
//             Log::Error("Cubemap texture failed to load at path: {}", paths[i].string());
//             stbi_image_free(data);
//             return;
//         }
//         if (width != height)
//         {
//             Log::Error("CubeMap must be square!");
//             stbi_image_free(data);
//             return;
//         }
//         GLenum format = GL_RGB;
//         if (nrChannels == 4)
//             format = GL_RGBA;
//         else if (nrChannels == 1)
//             format = GL_RED;
//
//         glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
//         stbi_image_free(data);
//
//         if (i == 0)
//         {
//             faceSize = width;
//         }
//     }
//     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//     glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
// }

 }
