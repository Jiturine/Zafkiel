#pragma once

#include "resource/asset.h"
#include "function/render/image.h"

namespace Zafkiel
{

enum class TextureWrap
{
    None = 0,
    Repeat,
    Clamp
};

enum class TextureFilter
{
    None = 0,
    Nearest,
    Linear
};

struct Texture2DSpecification
{
    uint32_t width;
    uint32_t height;
    ImageFormat format;
    std::vector<ImageUsage> usages;
    ImageUpdateFrequency updateFrequency;
    TextureWrap wrap = TextureWrap::Repeat;
    TextureFilter filter = TextureFilter::Nearest;
    uint32_t samples = 1;
};

class Texture
{
  public:
    virtual ~Texture() = default;
};

class Texture2DBackend
{
  public:
    virtual ~Texture2DBackend() = default;
};

class Texture2D final : public Texture
{
  public:
    Texture2D(const Texture2DSpecification &spec, RenderHandle image, Scope<Texture2DBackend> backend)
        : spec(spec), image(image), backend(std::move(backend)) {}

    RenderHandle GetImage() const { return image; }
  
    const Texture2DSpecification &GetSpecification() const { return spec; }

    Borrow<Texture2DBackend> GetBackend() const { return Borrow(backend); }

  private:
    Texture2DSpecification spec;
    RenderHandle image;
    Scope<Texture2DBackend> backend;
};

class CubeMap : public Texture
{
  public:
    virtual ~CubeMap() = default;
    virtual uint32_t GetFaceSize() const = 0;
};

}
