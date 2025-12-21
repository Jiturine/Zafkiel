#pragma once
#include "asset_metadata_serializer.h"
#include "core/meta/serializer/yaml_serializer.h"

namespace Zafkiel
{
  
class MaterialMetadataSerializer final : public AssetMetadataSerializer
{
  public:
    virtual Ref<EditorAssetMetadata> Deserialize(const IDeserializer &data, const Path &assetPath) override
    {
        auto handle = data["AssetHandle"].As<AssetHandle>();
        auto dependencies = data["Dependencies"].As<std::vector<AssetHandle>>();
        auto materialMetadata = CreateRef<EditorAssetMetadata>(handle, AssetType::Material, dependencies, assetPath);
        materialMetadata->detail.As<MaterialMetadata>().name = data["Name"].As<std::string>();
        return materialMetadata;
    }

    virtual std::string Serialize(const Ref<EditorAssetMetadata> &metadata) override
    {
        YamlSerializer serializer;
        serializer.BeginMap();
        serializer.Key("Type").Value(metadata->type);
        serializer.Key("AssetHandle").Value(metadata->handle);
        serializer.Key("Dependencies").Value(metadata->dependencies);
        serializer.Key("Name").Value(metadata->detail.As<MaterialMetadata>().name);
        serializer.EndMap();

        return serializer.c_str();
    }
};

}