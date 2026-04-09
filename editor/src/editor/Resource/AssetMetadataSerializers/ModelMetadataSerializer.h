
#pragma once
#include "editor/Resource/AssetMetadataSerializers/AssetMetadataSerializer.h"
#include "Core/Meta/Serializer/YamlSerializer.h"

namespace Zafkiel
{
  
class ModelMetadataSerializer final : public AssetMetadataSerializer
{
  public:
    virtual Ref<EditorAssetMetadata> Deserialize(const IDeserializer &data, const Path &assetPath) override
    {
        auto handle = data["AssetHandle"].As<AssetHandle>();
        auto dependencies = data["Dependencies"].As<std::vector<AssetHandle>>();
        auto modelMetadata = CreateRef<EditorAssetMetadata>(handle, AssetType::Model, dependencies, assetPath);
        return modelMetadata;
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