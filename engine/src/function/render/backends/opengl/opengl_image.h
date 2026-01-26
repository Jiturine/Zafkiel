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
    OpenGLImageBackend(uint32_t rendererID) : rendererID(rendererID) {}
    ~OpenGLImageBackend();
    
    uint32_t GetRendererID() const { return rendererID; }

  private:
    uint32_t rendererID;
};

}
