#pragma once
#include "function/render/vertex_buffer.h"
#include <glad/glad.h>

namespace Zafkiel
{

GLenum ShaderDataTypeToOpenGLBaseType(ShaderFundamentalType type);
uint32_t ShaderDataTypeCount(ShaderFundamentalType type);

class OpenGLVertexBufferBackend final : public VertexBufferBackend
{
  public:
    OpenGLVertexBufferBackend(uint32_t rendererID) : rendererID(rendererID) {}
    ~OpenGLVertexBufferBackend();

    uint32_t GetRendererID() const { return rendererID; }

  private:
    uint32_t rendererID;
};

}
