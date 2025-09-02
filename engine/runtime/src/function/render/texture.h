#pragma once

#include "resource/asset.h"

namespace Zafkiel
{

enum class ImageFormat
{
    None = 0,
    RGB8,
    RGBA8
};

struct TextureSpecification
{
    uint32_t width;
    uint32_t height;
    ImageFormat format;
};

class Texture : public Asset
{
  public:
    virtual ~Texture() = default;
    virtual uint32_t GetWidth() const = 0;
    virtual uint32_t GetHeight() const = 0;
    virtual uint32_t GetRendererID() const = 0;
    virtual void SetData(const Buffer &buffer) = 0;

    virtual void Bind(uint32_t slot = 0) const = 0;
};

class Texture2D : public Texture
{
  public:
    virtual ~Texture2D() = default;
    virtual AssetType GetType() const override { return AssetType::Texture2D; }
};

}