#pragma once
#include "function/render/frame_buffer.h"

namespace Zafkiel
{
class OpenGLFrameBufferBackend final : public FrameBufferBackend
{
  public:
    OpenGLFrameBufferBackend(const FrameBufferSpecification &spec);
    ~OpenGLFrameBufferBackend();

    virtual void Resize(uint32_t width, uint32_t height) override;
    
    uint32_t GetRendererID() const { return rendererID; }

  private:
    void Invalidate();

    uint32_t rendererID = 0;
    uint32_t width;
    uint32_t height;
    std::vector<Observer<Image>> attachments;
};

class OpenGLFrameBufferFactory
{
  public:
    static Scope<FrameBuffer> Create(const FrameBufferSpecification &spec);
};

}
