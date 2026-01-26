#pragma once
#include "function/render/frame_buffer.h"
#include "function/render/render_registry.h"

namespace Zafkiel
{
class OpenGLFrameBufferBackend final : public FrameBufferBackend
{
  public:
    OpenGLFrameBufferBackend(uint32_t rendererID) : rendererID(rendererID) {}
    ~OpenGLFrameBufferBackend();
    
    uint32_t GetRendererID() const { return rendererID; }

  private:
    uint32_t rendererID = 0;
};

}
