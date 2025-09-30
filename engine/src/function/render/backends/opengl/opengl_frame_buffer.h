#pragma once
#include "function/render/frame_buffer.h"

namespace Zafkiel
{
class OpenGLFrameBuffer : public FrameBuffer
{
  public:
    OpenGLFrameBuffer(const FrameBufferSpecification &spec);
    ~OpenGLFrameBuffer();
    virtual const FrameBufferSpecification &GetSpecification() const override { return spec; }
    virtual Ref<Texture2D> GetColorAttachment(uint32_t index = 0) const override;
    virtual void Bind() const override;
    virtual void Unbind() const override;
    virtual void Resize(uint32_t width, uint32_t height) override;
    virtual uint32_t ReadPixelUInt(uint32_t attachmentIndex, int x, int y) override;
    virtual void ClearColorAttachment(uint32_t index, const void *value) override;
    virtual uint32_t GetRendererID() const override { return rendererID; }

  private:
    void Invalidate();

    uint32_t rendererID = 0;
    FrameBufferSpecification spec;
    std::vector<Ref<Texture2D>> colorAttachments;
    Ref<Texture2D> depthAttachment;
};
}