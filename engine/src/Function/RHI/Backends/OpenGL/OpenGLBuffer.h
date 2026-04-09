#pragma once
#include "Function/RHI/RHIResources.h"
#include <glad/glad.h>

namespace Zafkiel
{

GLenum ShaderDataTypeToOpenGLBaseType(ShaderFundamentalType type);

uint32 ShaderDataTypeCount(ShaderFundamentalType type);

class OpenGLBuffer final : public RHIBuffer
{
  public:
    OpenGLBuffer(const RHIBufferDesc &desc, const void *data = nullptr);

    ~OpenGLBuffer();

    GLuint GetHandle() const { return handle; }

    GLenum GetType() const;

  private:
    GLuint handle;

};

}
