#pragma once
#include "function/render/render_handle.h"

namespace Zafkiel
{

enum class ImageFormat
{
    None = 0,
    R8,
    RG8,
    RGB8,
    RGBA8,
    BGR8,
    BGRA8,
    R8_sRGB,
    RG8_sRGB,
    RGB8_sRGB,
    RGBA8_sRGB,
    BGR8_sRGB,
    BGRA8_sRGB,
    RGB16F,
    RGBA16F,
    RGBA32F,
    R32UI,
    R32F,
    DEPTH24STENCIL8,
    DEPTH32F,
};

uint32_t ImageFormatToBytes(ImageFormat format);

enum class ImageUsage
{
    None = 0,
    Upload, // 上传图像数据
    Sampled,
    ColorAttachment,
    DepthAttachment
};

enum class ImageLayout
{
    Undefined = 0,
    ShaderReadOnly,
    ColorAttachment,
    DepthAttachment,
    DepthStencilAttachment,
    PresentSrc
};

enum class ImageUpdateFrequency
{
    Static,
    Dynamic,
    Transient
};

struct ImageSpecification
{
    ImageFormat format;
    std::vector<ImageUsage> usages;
    ImageUpdateFrequency updateFrequency;
    uint32_t width;
    uint32_t height;
    uint32_t samples = 1;
};

class ImageBackend
{
  public:
    virtual ~ImageBackend() = default;
};

class Image final
{
  public:
    Image(const ImageSpecification &spec, Scope<ImageBackend> backend)
        : backend(std::move(backend)), spec(spec) {}

    uint32_t GetWidth() const { return spec.width; }
    uint32_t GetHeight() const { return spec.height; }
    ImageFormat GetFormat() const { return spec.format; }
    Borrow<ImageBackend> GetBackend() const { return Borrow(backend); }

    const ImageSpecification &GetSpecification() const { return spec; }

  private:
    ImageSpecification spec;
    Scope<ImageBackend> backend;

};

}
