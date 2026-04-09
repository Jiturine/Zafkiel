
#pragma once
#include "editor/Resource/AssetMetadataSerializers/AssetMetadataSerializer.h"
#include "Core/Meta/Serializer/YamlSerializer.h"

namespace Zafkiel
{
  
class Texture2DMetadataSerializer final : public AssetMetadataSerializer
{
  public:
    virtual Ref<EditorAssetMetadata> Deserialize(const IDeserializer &data, const Path &assetPath) override
    {
        auto handle = data["AssetHandle"].As<AssetHandle>();
        auto dependencies = data["Dependencies"].As<std::vector<AssetHandle>>();
        auto texture2DMetadata = CreateRef<EditorAssetMetadata>(handle, AssetType::Texture2D, dependencies, assetPath);
        texture2DMetadata->detail.As<Texture2DMetadata>().format = data["Format"].As<ImageFormat>();
        return texture2DMetadata;
    }

    virtual std::string Serialize(const Ref<EditorAssetMetadata> &metadata) override
    {
        YamlSerializer serializer;
        serializer.BeginMap();
        serializer.Key("Type").Value(metadata->type);
        serializer.Key("AssetHandle").Value(metadata->handle);
        serializer.Key("Dependencies").Value(metadata->dependencies);
        serializer.Key("Format").Value(metadata->detail.As<Texture2DMetadata>().format);
        serializer.EndMap();

        return serializer.c_str();
    }
};

}