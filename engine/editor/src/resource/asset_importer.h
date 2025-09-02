#pragma once

#include "function/render/graphics_context.h"
#include "function/render/texture.h"
#include <stb_image.h>
#include "asset_metadata.h"

namespace Zafkiel
{
class AssetImporter : public RefCounted
{
  public:
    AssetImporter(Ref<GraphicsContext> context) : context(context) {}
    Ref<Asset> ImportAsset(const AssetMetadata &metadata) const;
  private:
    Ref<GraphicsContext> context;

    Ref<Asset> ImportTexture2D(const Path &path) const;
};
}