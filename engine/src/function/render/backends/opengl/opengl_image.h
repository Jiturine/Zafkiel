#pragma once
#include "function/render/image.h"
#include <glad/glad.h>

namespace Zafkiel
{

struct OpenGLImageFormat
{
    GLenum internalFormat;
    GLenum dataFormat;
    GLenum dataType;
};

OpenGLImageFormat ImageFormatToOpenGLType(ImageFormat format);

class OpenGLImageBackend final : public ImageBackend
{
  public:
    OpenGLImageBackend(const ImageSpecification &spec);
    ~OpenGLImageBackend();

    virtual void Resize(uint32_t width, uint32_t height) override;
    
    uint32_t GetRendererID() const { return rendererID; }

  private:
    ImageFormat format;
    uint32_t rendererID;
};

class OpenGLImageFactory
{
  public:
    static Scope<Image> Create(const ImageSpecification &spec);
};

}
