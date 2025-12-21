#pragma once
#include "asset_metadata_serializer.h"
#include "core/meta/serializer/yaml_serializer.h"

namespace Zafkiel
{
  
class SceneMetadataSerializer final : public AssetMetadataSerializer
{
  public:
    virtual Ref<EditorAssetMetadata> Deserialize(const IDeserializer &data, const Path &assetPath) override
    {
        auto handle = data["AssetHandle"].As<AssetHandle>();
        auto dependencies = data["Dependencies"].As<std::vector<AssetHandle>>();
        auto materialMetadata = CreateRef<EditorAssetMetadata>(handle, AssetType::Scene, dependencies, assetPath);
        return materialMetadata;
    }

    virtual std::string Serialize(const Ref<EditorAssetMetadata> &metadata) override
    {
        YamlSerializer serializer;
        serializer.BeginMap();
        serializer.Key("Type").Value(metadata->type);
        serializer.Key("AssetHandle").Value(metadata->handle);
        serializer.Key("Dependencies").Value(metadata->dependencies);
        serializer.EndMap();

        return serializer.c_str();
    }
};

}