#pragma once

#include "assimp/scene.h"
#include "editor/Resource/AssimpImporter.h"
#include "editor/Resource/AssimpImporter.h"
#include "editor/Resource/EditorAsset.h"
#include "Function/Render/Model.h"
#include "Resource/AssetManager.h"
#include "Platform/Filesystem/Filesystem.h"
#include "Core/Meta/Serializer/CustomSerialize.h"
#include "Resource/MaterialAsset.h"
#include "Resource/ModelAsset.h"
#include "Resource/SceneAsset.h"
#include "Resource/Texture2DAsset.h"
#include "editor/Resource/AssetLoaders/AssetLoader.h"

namespace Zafkiel
{

class EditorAssetManager final : public AssetManager
{
  public:
    // AssetHandle GetBuiltinShader(BuiltinShaderType type);

    static void Init()
    {
        instance = new EditorAssetManager;
        AssetManager::instancePtr = instance;
    }
    static void Destroy()
    {
        delete instance;
        instance = nullptr;
        AssetManager::instancePtr = nullptr;
    }

    virtual Ref<Asset> GetAsset(AssetHandle handle) override;
    virtual Ref<AssetMetadata> GetAssetMetadata(AssetHandle handle) override;
    virtual bool IsAssetValid(AssetHandle handle) const override;
    virtual bool IsAssetLoaded(AssetHandle handle) const override;
    virtual Ref<Asset> LoadAsset(AssetHandle handle) override;

    void SetAssetDirectory(const Path &path);
    Path GetAssetDirectory();
    AssetHandle GetRegisterdAsset(const Path &assetPath) const;
    void RegisterAssets();
    void RegisterAsset(const Path &metadataPath);
    bool IsFileRegisterd(const Path &path) const;
    
    AssetHandle ImportAsset(const Path &assetPath);
    AssetHandle ImportMaterial(const Path &path);
    AssetHandle ImportModel(const Path &path, ShaderFamily shaderFamily = ShaderFamily::BlinnPhong);
    AssetHandle ImportMesh(const Path &path);
    AssetHandle ImportFont(const Path &path);
    AssetHandle ImportTexture2D(const Path &path, ImageFormat format = ImageFormat::RGBA8);
    AssetHandle ImportScene(const Path &path);

    void LoadBuiltinShader();

    static EditorAssetManager &Instance() { return *instance; }
    
    friend class MaterialImporter;
    friend class ModelImporter;
    friend class ModelLoader;
    friend class ModelSerializer;

  private:
    inline static EditorAssetManager *instance = nullptr;

    std::unordered_map<AssetHandle, Ref<Asset>> loadedAssets;
    std::unordered_map<AssetHandle, Ref<EditorAssetMetadata>> assetRegistry;
    std::unordered_map<std::string, AssetHandle> registeredFiles;
    Path assetDirectory;

};

}
