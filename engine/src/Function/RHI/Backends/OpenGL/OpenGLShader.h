#pragma once
#include "Function/RHI/RHIResources.h"
#include <glad/glad.h>

namespace Zafkiel
{
class OpenGLShader
{
  public:
    OpenGLShader(Buffer code, ShaderType shaderType, ShaderResourceTable &shaderResourceTable);

    ~OpenGLShader();

    GLuint GetHandle() const { return handle; }

  private:
    ShaderResourceTable &SRT;
    GLuint handle;
};

class OpenGLVertexShader : public RHIVertexShader, public OpenGLShader
{
  public:
    OpenGLVertexShader(Buffer code, Scope<ShaderResourceTable> SRT);
};

class OpenGLFragmentShader : public RHIFragmentShader, public OpenGLShader
{
  public:
    OpenGLFragmentShader(Buffer code, Scope<ShaderResourceTable> SRT)
        : RHIFragmentShader(MoveTemp(SRT)),
          OpenGLShader(code, ShaderType::Fragment, *shaderResourceTable.get()) {}
};

}
