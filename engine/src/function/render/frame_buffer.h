#pragma once
#include "texture.h"

namespace Zafkiel
{

struct FrameBufferSpecification
{
    uint32_t width, height;
    std::vector<TextureFormat> attachments;
};

class FrameBuffer : public RefCounted
{
  public:
    virtual ~FrameBuffer() = default;
    virtual const FrameBufferSpecification &GetSpecification() const = 0;
    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;

    virtual void Resize(uint32_t width, uint32_t height) = 0;

    template <typename T>
    T ReadPixel(uint32_t attachmentIndex, int x, int y);

    virtual void ClearColorAttachment(uint32_t index, const void *value) = 0;

    virtual Ref<Texture2D> GetColorAttachment(uint32_t index = 0) const = 0;

    virtual uint32_t GetRendererID() const = 0;

  protected:
    virtual uint32_t ReadPixelUInt(uint32_t attachmentIndex, int x, int y) = 0;
};

}