#pragma once
#include "function/render/index_buffer.h"

namespace Zafkiel
{

class OpenGLIndexBufferBackend final : public IndexBufferBackend
{
  public:
    OpenGLIndexBufferBackend(uint32_t rendererID, uint32_t count)
        : rendererID(rendererID), count(count) {}
    ~OpenGLIndexBufferBackend();

    uint32_t GetRendererID() const { return rendererID; }

  private:
    uint32_t rendererID;
    uint32_t count;
};

}
