#pragma once

#include "assimp/scene.h"
#include "editor/resource/assimp_importer.h"
#include "editor/resource/assimp_importer.h"
#include "editor/resource/editor_asset.h"
#include "function/render/material.h"
#include "function/render/model.h"
#include "resource/asset_manager.h"
#include "platform/filesystem/filesystem.h"
#include "function/render/graphics_context.h"
#include "core/meta/serializer/custom_serialize.h"
#include "resource/material_asset.h"
#include "resource/model_asset.h"
#include "resource/scene_asset.h"
#include "resource/texture2d_asset.h"
#include "editor/resource/asset_loaders/asset_loader.h"

namespace Zafkiel
{

enum class BuiltinShaderType
{
    Unlit
};

class EditorAssetManager final : public AssetManager
{
  public:
    static void SetAssetDirectory(const Path &path) { instance->SetAssetDirectoryImpl(path); }
    static Path GetAssetDirectory() { return instance->GetAssetDirectoryImpl(); }
    static AssetHandle ImportAsset(const Path &assetPath) { return instance->ImportAssetImpl(assetPath); }
    static AssetHandle GetRegisterdAsset(const Path &assetPath) { return instance->GetRegisterdAssetImpl(assetPath); }
    static void RegisterAssets() { return instance->RegisterAssetsImpl(); }
    static void RegisterAsset(const Path &metadataPath) { return instance->RegisterAssetImpl(metadataPath); }
    static bool IsFileRegisterd(const Path &path) { return instance->IsFileRegisterdImpl(path); }

    AssetHandle GetBuiltinShader(BuiltinShaderType type);

    static void LoadBuiltinShader() { instance->LoadBuiltinShaderImpl(); }

    static void Init()
    {
        instance.reset(new EditorAssetManager);
        AssetManager::instancePtr = instance;
    }
    static void Destroy()
    {
        instance = nullptr;
        AssetManager::instancePtr = nullptr;
    }

    static Observer<EditorAssetManager> Instance() { return instance; }
    
    friend class MaterialImporter;
    friend class ModelImporter;
    friend class ModelLoader;
    friend class ModelSerializer;

    inline static const AssetHandle unlitShaderHandle = 1111111111111111111;
  private:
    inline static Scope<EditorAssetManager> instance;

    virtual Ref<Asset> GetAssetImpl(AssetHandle handle) override;
    virtual Ref<AssetMetadata> GetAssetMetadataImpl(AssetHandle handle) override;
    virtual bool IsAssetValidImpl(AssetHandle handle) const override;
    virtual bool IsAssetLoadedImpl(AssetHandle handle) const override;
    virtual Ref<Asset> LoadAssetImpl(AssetHandle handle) override;

    void SetAssetDirectoryImpl(const Path &path);
    Path GetAssetDirectoryImpl();
    AssetHandle ImportAssetImpl(const Path &assetPath);
    AssetHandle GetRegisterdAssetImpl(const Path &assetPath) const;
    void RegisterAssetsImpl();
    void RegisterAssetImpl(const Path &metadataPath);
    bool IsFileRegisterdImpl(const Path &path) const ;

    void LoadBuiltinShaderImpl();

    std::unordered_map<AssetHandle, Ref<Asset>> loadedAssets;
    std::unordered_map<AssetHandle, Ref<EditorAssetMetadata>> assetRegistry;
    std::unordered_map<std::string, AssetHandle> registeredFiles;
    Path assetDirectory;

};

}
