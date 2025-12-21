#pragma once
#include "core/base/memory.h"
#include "function/render/texture.h"
#include "resource/asset.h"

namespace Zafkiel
{

class Texture2DAsset : public Asset
{
  public:
    virtual AssetType GetAssetType() const override { return AssetType::Texture2D; }

    Texture2DAsset(AssetHandle handle, const Texture2DSpecification &spec, ScopedBuffer buffer)
        : Asset(handle), buffer(std::move(buffer)), spec(spec)
    {
        // Renderer::Submit([self = Ref(this), spec, buffer = std::move(buffer)]() mutable {
        //     self->texture2D = Renderer::GetGraphicsContext()->CreateTexture2D(spec, buffer);
        // });
    }

    ~Texture2DAsset()
    {
        // Renderer::Submit([texture2D = std::move(texture2D)]() mutable {
        //     texture2D = nullptr;
        // });
    }

    const Texture2DSpecification &GetSpecification() const { return spec; }

    Buffer GetBuffer() const { return buffer; }

  private:
    Texture2DSpecification spec;
    ScopedBuffer buffer;
};

}
