#pragma once
#include "../../frame_buffer.h"

namespace Zafkiel
{
class OpenGLFrameBuffer : public FrameBuffer
{
  public:
    OpenGLFrameBuffer(const FrameBufferSpecification &spec);
    ~OpenGLFrameBuffer();
    void Invalidate();
    virtual const FrameBufferSpecification &GetSpecification() const override { return specification; }
    virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override;
    virtual void Bind() const override;
    virtual void Unbind() const override;
    virtual void Resize(uint32_t width, uint32_t height) override;
    virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) override;
    virtual void ClearAttachment(uint32_t attachmentIndex, int value) override;

  private:
    uint32_t rendererID = 0;
    FrameBufferSpecification specification;
    std::vector<FrameBufferTextureSpecification> colorAttachmentSpecifications;
    FrameBufferTextureSpecification depthAttachmentSpecification = FrameBufferTextureFormat::None;
    std::vector<uint32_t> colorAttachments;
    uint32_t depthAttachment = 0;
};
}