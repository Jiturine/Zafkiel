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
    OpenGLVertexBufferBackend(const float *vertices, uint32_t size);
    ~OpenGLVertexBufferBackend();

    uint32_t GetRendererID() const { return rendererID; }

  private:
    uint32_t rendererID;
};

class OpenGLVertexBufferFactory
{
  public:
    static Scope<VertexBuffer> Create(const float *vertices, uint32_t size);
};

}
