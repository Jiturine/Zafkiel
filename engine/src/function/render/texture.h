#pragma once

#include "resource/asset.h"
#include "image.h"

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
    Texture2D(const Texture2DSpecification &spec, Scope<Texture2DBackend> backend)
        : backend(std::move(backend)) {}
    uint32_t GetWidth() const { return image->GetWidth(); }
    uint32_t GetHeight() const { return image->GetHeight(); }
    const Observer<Image> GetImage() const { return image; }
    Observer<Texture2DBackend> GetBackend() { return backend; }
    const Observer<Texture2DBackend> GetBackend() const { return backend; }

    void Resize(uint32_t width, uint32_t height) { image->Resize(width, height); }
    
    template<typename Derived>
    friend class Texture2DFactory;

  private:
    Scope<Image> image;
    Scope<Texture2DBackend> backend;
};

template<typename Derived>
class Texture2DFactory
{
  protected:
    static Scope<Image> &AccessImage(const Scope<Texture2D> &texture2D)
    {
        return texture2D->image;
    }
};

class CubeMap : public Texture
{
  public:
    virtual ~CubeMap() = default;
    virtual uint32_t GetFaceSize() const = 0;
};

}
