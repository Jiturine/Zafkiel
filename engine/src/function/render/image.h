#pragma once

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
    DEPTH24STENCIL8
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
    virtual void Resize(uint32_t width, uint32_t height) = 0;
};

class Image final
{
  public:
    Image(const ImageSpecification &spec, Scope<ImageBackend> backend)
        : backend(std::move(backend)), width(spec.width), height(spec.height), format(spec.format)
    {
    }
    uint32_t GetWidth() const { return width; }
    uint32_t GetHeight() const { return height; }
    ImageFormat GetFormat() const { return format; }
    Observer<ImageBackend> GetBackend() { return backend; }
    const Observer<ImageBackend> GetBackend() const { return backend; }

    void Resize(uint32_t width, uint32_t height)
    {
        this->width = width;
        this->height = height;
        backend->Resize(width, height);
    }

  private:
    uint32_t width;
    uint32_t height;
    ImageFormat format;
    Scope<ImageBackend> backend;

};

}
