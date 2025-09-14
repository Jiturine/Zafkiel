#pragma once

namespace Zafkiel
{
enum class FrameBufferTextureFormat
{
    None = 0,
    RGBA8,
    RGBA16F,
    RGBA32F,
    RED_INTEGER,
    DEPTH24STENCIL8,
    Depth = DEPTH24STENCIL8
};
struct FrameBufferTextureSpecification
{
    FrameBufferTextureSpecification() = default;
    FrameBufferTextureSpecification(FrameBufferTextureFormat format)
        : textureFormat(format) {}
    FrameBufferTextureFormat textureFormat = FrameBufferTextureFormat::None;
    // bool multisample = false;
    // uint32_t samples = 1;
};
struct FrameBufferSpecification
{
    uint32_t width, height;
    std::vector<FrameBufferTextureSpecification> attachments;
    uint32_t samples = 1;
    bool swapChainTarget = false;
};

class FrameBuffer : public RefCounted
{
  public:
    virtual ~FrameBuffer() = default;
    virtual const FrameBufferSpecification &GetSpecification() const = 0;
    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;

    virtual void Resize(uint32_t width, uint32_t height) = 0;
    virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) = 0;

    virtual void ClearAttachment(uint32_t attachmentIndex, int value) = 0;

    virtual uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const = 0;
};
}