#pragma once
#include "editor/Resource/EditorAsset.h"

namespace Zafkiel
{

class AssetMetadataSerializer
{
  public:
    virtual ~AssetMetadataSerializer() = default;
    virtual Ref<EditorAssetMetadata> Deserialize(const IDeserializer &data, const Path &assetPath) = 0;
    virtual std::string Serialize(const Ref<EditorAssetMetadata> &metadata) = 0;

};

}