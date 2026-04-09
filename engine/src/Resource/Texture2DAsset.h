#pragma once
#include "Resource/Asset.h"
#include "Function/RHI/RHIResources.h"

namespace Zafkiel
{

struct Texture2DDesc
{
    uint32 width;
    uint32 height;
    ImageFormat format;
    ImageUsageFlags usages = ImageUsageFlags::Sampled | ImageUsageFlags::Upload;
    TextureWrap wrap = TextureWrap::Repeat;
    TextureFilter filter = TextureFilter::Nearest;
    uint32 sampleCount = 1;
};

class Texture2DAsset : public Asset
{
  public:
    virtual AssetType GetAssetType() const override { return AssetType::Texture2D; }

    Texture2DAsset(AssetHandle handle, const Texture2DDesc &desc, ScopedBuffer buffer)
        : Asset(handle), buffer(MoveTemp(buffer)), desc(desc) {}

    ~Texture2DAsset() = default;

    uint32 GetWidth() const { return desc.width; }
    uint32 GetHeight() const { return desc.height; }
    ImageFormat GetFormat() const { return desc.format; }
    ImageUsageFlags GetUsages() const { return desc.usages; }
    TextureWrap GetWrap() const { return desc.wrap; }
    TextureFilter GetFilter() const { return desc.filter; }
    uint32 GetSampleCount() const { return desc.sampleCount; }

    Buffer GetBuffer() const { return buffer; }

  private:
    Texture2DDesc desc;

    ScopedBuffer buffer;
};

}
