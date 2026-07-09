#pragma once
#include "Function/RHI/RHIResources.h"
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

class OpenGLRHI;
  
class OpenGLTexture final : public RHITexture
{
  public:
    OpenGLTexture(OpenGLRHI &rhi, const RHITextureDesc &desc, Buffer data = nullptr);

    ~OpenGLTexture();

    GLuint GetHandle() const { return handle; }
  
  private:
    OpenGLRHI &rhi;

    GLuint handle;
};

}
