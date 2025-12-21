#pragma once
#include "function/render/index_buffer.h"

namespace Zafkiel
{

class OpenGLIndexBufferBackend final : public IndexBufferBackend
{
  public:
    OpenGLIndexBufferBackend(const uint32_t *indices, uint32_t count);
    ~OpenGLIndexBufferBackend();

    uint32_t GetRendererID() const { return rendererID; }

  private:
    uint32_t rendererID;
    uint32_t count;
};

class OpenGLIndexBufferFactory
{
  public:
    static Scope<IndexBuffer> Create(const uint32_t* indices, uint32_t count);
};

}
