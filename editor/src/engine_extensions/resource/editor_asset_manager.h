#pragma once

#include "assimp/scene.h"
#include "resource/asset_manager.h"
#include "platform/filesystem/filesystem.h"
#include "function/render/graphics_context.h"
#include "core/meta/serializer/custom_serialize.h"
#include "core/meta/serializer/deserialize.h"

namespace Zafkiel
{
struct AssetMetadata : public RefCounted
{
    AssetType type;
    Path filePath;
    AssetHandle handle;
};

struct MeshMetadata : public AssetMetadata
{
    Path nodePath;
    AssetHandle parentModel;
    mat4 transform;
};

struct ModelMetadata : public AssetMetadata
{
    std::unordered_map<AssetHandle, Ref<MeshMetadata>> meshes;
};

struct Texture2DMetadata : public AssetMetadata
{
};

class [[refl]] AssetRegistry
{
  public:
    friend class EditorAssetManager;
    friend struct Serialization<AssetRegistry>;

  private:
    std::unordered_map<AssetHandle, Ref<AssetMetadata>> metadatas;
    std::unordered_map<AssetHandle, AssetHandle> parentMap;
};

template <>
struct Serialization<AssetRegistry>
{
    static constexpr bool has_serialize = true;
    static void Serialize(const Any instance, Any context, YAML::Emitter &out)
    {
        const auto &assetRegistry = instance.As<AssetRegistry>();
        out << YAML::BeginSeq;
        for (auto &[assetHandle, metadata] : assetRegistry.metadatas)
        {
            if (metadata->type == AssetType::Mesh) continue;
            out << YAML::BeginMap;
            out << YAML::Key << "AssetHandle" << YAML::Value;
            SerializeAny(assetHandle, GetType<UUID>(), out);
            out << YAML::Key << "Type" << YAML::Value;
            SerializeAny(metadata->type, GetType<AssetType>(), out);
            out << YAML::Key << "FilePath" << YAML::Value;
            SerializeAny(metadata->filePath, GetType<Path>(), out);
            if (metadata->type == AssetType::Model)
            {
                const auto &modelMetadata = metadata.As<ModelMetadata>();
                out << YAML::Key << "Meshes" << YAML::Value << YAML::BeginSeq;
                for (auto &[meshHandle, meshMetadata] : modelMetadata->meshes)
                {
                    out << YAML::BeginMap;
                    out << YAML::Key << "AssetHandle" << YAML::Value;
                    SerializeAny(meshHandle, GetType<UUID>(), out);
                    out << YAML::Key << "NodePath" << YAML::Value;
                    SerializeAny(meshMetadata->nodePath, GetType<Path>(), out);
                    out << YAML::EndMap;
                }
                out << YAML::EndSeq;
            }
            out << YAML::EndMap;
        }
        out << YAML::EndSeq;
    }
    static void Deserialize(Any instance, Any context, const YAML::Node &data)
    {
        auto &assetRegistry = instance.As<AssetRegistry>();
        for (auto &assetMetadataNode : data)
        {
            AssetHandle assetHandle;
            DeserializeAny(assetHandle, GetType<AssetHandle>(), assetMetadataNode["AssetHandle"]);

            AssetType type;
            DeserializeAny(type, GetType<AssetType>(), assetMetadataNode["Type"]);

            Path filePath;
            DeserializeAny(filePath, GetType<Path>(), assetMetadataNode["FilePath"]);

            if (type == AssetType::Model)
            {
                auto &meshNodes = assetMetadataNode["Meshes"];

                Ref<ModelMetadata> modelMetadata = MakeRef<ModelMetadata>();

                modelMetadata->type = AssetType::Model;
                modelMetadata->filePath = filePath;

                for (auto &meshMetadataNode : meshNodes)
                {
                    Ref<MeshMetadata> meshMetadata = MakeRef<MeshMetadata>();
                    meshMetadata->type = AssetType::Mesh;
                    meshMetadata->filePath = filePath;

                    Path nodePath;
                    DeserializeAny(nodePath, GetType<Path>(), meshMetadataNode["NodePath"]);
                    meshMetadata->nodePath = nodePath;

                    meshMetadata->parentModel = assetHandle;

                    AssetHandle meshHandle;
                    DeserializeAny(meshHandle, GetType<AssetHandle>(), meshMetadataNode["AssetHandle"]);
                    modelMetadata->meshes[meshHandle] = meshMetadata;
                    assetRegistry.parentMap[meshHandle] = assetHandle;
                    assetRegistry.metadatas[meshHandle] = meshMetadata;
                }
                assetRegistry.metadatas[assetHandle] = modelMetadata;
            }
            else if (type == AssetType::Texture2D)
            {
                Ref<Texture2DMetadata> texture2DMetadata = MakeRef<Texture2DMetadata>();
                texture2DMetadata->type = AssetType::Texture2D;
                texture2DMetadata->filePath = filePath;

                assetRegistry.metadatas[assetHandle] = texture2DMetadata;
            }
            else
            {
                Log::CoreError("Unknown Asset Type!");
            }
        }
    }
};

class EditorAssetManager : public AssetManager
{
  public:
    EditorAssetManager();
    void SetAssetDirectory(const Path &path);
    Path GetAssetDirectory();
    AssetHandle ImportAsset(const Path &assetPath);
    void SerializeAssetRegistry();
    void DeserializeAssetRegistry();
    virtual Ref<Asset> GetAsset(AssetHandle handle) override;
    virtual bool IsAssetValid(AssetHandle handle) const override;
    virtual bool IsAssetLoaded(AssetHandle handle) const override;

  private:
    Ref<Asset> LoadAsset(AssetHandle handle, const Ref<AssetMetadata> &metadata);
    Ref<Asset> LoadTexture2D(const Ref<Texture2DMetadata> &metadata) const;
    void LoadNode(aiNode *node, const aiScene *scene, const mat4 &parentTransform, std::vector<Ref<Mesh>> &meshes, Path nodePath, Ref<ModelMetadata> modelMetadata);
    Ref<Asset> LoadModel(const Ref<ModelMetadata> &metadata);

    AssetHandle ImportTexture2D(const Path &assetPath);
    AssetHandle ImportModel(const Path &assetPath);

    std::unordered_map<AssetHandle, Ref<Asset>> loadedAssets;
    AssetRegistry assetRegistry;
    Path assetDirectory;
};

}