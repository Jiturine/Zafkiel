#include "core/base/utils.h"
#include "editor/resource/editor_asset_manager.h"
#include "core/meta/serializer/yaml_serializer.h"
#include "function/render/model.h"
#include "function/render/surface_material.h"
#include "function/render/renderer.h"
#include "function/scene/scene.h"
#include "resource/scene_asset.h"
#include "resource/shader_asset.h"
#include "resource/texture2d_asset.h"
#include "stb_image.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "core/base/buffer.h"
#include "platform/filesystem/filesystem.h"
#include "resource/texture2d_asset.h"
#include "resource/material_asset.h"

#include "editor/resource/asset_importers/mesh_importer.h"
#include "editor/resource/asset_importers/model_importer.h"
#include "editor/resource/asset_importers/texture2d_importer.h"
#include "editor/resource/asset_importers/scene_importer.h"
#include "editor/resource/asset_importers/material_importer.h"

#include "editor/resource/asset_loaders/mesh_loader.h"
#include "editor/resource/asset_loaders/model_loader.h"
#include "editor/resource/asset_loaders/material_loader.h"
#include "editor/resource/asset_loaders/scene_loader.h"
#include "editor/resource/asset_loaders/texture2d_loader.h"

#include "editor/resource/asset_metadata_serializers/mesh_metadata_serializer.h"
#include "editor/resource/asset_metadata_serializers/model_metadata_serializer.h"
#include "editor/resource/asset_metadata_serializers/material_metadata_serializer.h"
#include "editor/resource/asset_metadata_serializers/scene_metadata_serializer.h"
#include "editor/resource/asset_metadata_serializers/texture2d_metadata_serializer.h"

namespace Zafkiel
{
    
static std::unordered_map<AssetType, std::function<Scope<AssetLoader>()>> createAssetLoaderFuncs
{
    {AssetType::Material, [](){ return CreateScope<MaterialLoader>(); } },
    {AssetType::Model, [](){ return CreateScope<ModelLoader>(); } },
    {AssetType::Mesh, [](){ return CreateScope<MeshLoader>(); } },
    {AssetType::Scene, [](){ return CreateScope<SceneLoader>(); } },
    {AssetType::Texture2D, [](){ return CreateScope<Texture2DLoader>(); } },
};

static std::unordered_map<AssetType, std::function<Scope<AssetImporter>()>> createAssetImporterFuncs
{
    {AssetType::Material, [](){ return CreateScope<MaterialImporter>(); } },
    {AssetType::Model, [](){ return CreateScope<ModelImporter>(); } },
    {AssetType::Mesh, [](){ return CreateScope<MeshImporter>(); } },
    {AssetType::Scene, [](){ return CreateScope<SceneImporter>(); } },
    {AssetType::Texture2D, [](){ return CreateScope<Texture2DImporter>(); } },
};

static std::unordered_map<AssetType, std::function<Scope<AssetMetadataSerializer>()>> createAssetMetadataSerializerFuncs
{
    {AssetType::Material, [](){ return CreateScope<MaterialMetadataSerializer>(); } },
    {AssetType::Model, [](){ return CreateScope<ModelMetadataSerializer>(); } },
    {AssetType::Mesh, [](){ return CreateScope<MeshMetadataSerializer>(); } },
    {AssetType::Scene, [](){ return CreateScope<SceneMetadataSerializer>(); } },
    {AssetType::Texture2D, [](){ return CreateScope<Texture2DMetadataSerializer>(); } },
};

bool EditorAssetManager::IsAssetValidImpl(AssetHandle handle) const
{
    return assetRegistry.contains(handle);
}
bool EditorAssetManager::IsAssetLoadedImpl(AssetHandle handle) const
{
    return loadedAssets.contains(handle);
}
bool EditorAssetManager::IsFileRegisterdImpl(const Path &path) const
{
    return registeredFiles.contains(path);
}
AssetHandle EditorAssetManager::GetRegisterdAssetImpl(const Path &assetPath) const
{
    return registeredFiles.at(assetPath);
}

// 尽量延迟加载依赖项
Ref<Asset> EditorAssetManager::GetAssetImpl(AssetHandle handle)
{
    auto metadata = GetAssetMetadataImpl(handle).As<EditorAssetMetadata>();
    if (!metadata) return nullptr;

    if (auto it = loadedAssets.find(handle); it != loadedAssets.end())
    {
        return it->second;
    }

    Scope<AssetLoader> loader = createAssetLoaderFuncs[metadata->type]();
    auto asset = loader->Load(metadata);
    loadedAssets[handle] = asset;

    return asset;
}

// 主动加载所有依赖资源
Ref<Asset> EditorAssetManager::LoadAssetImpl(AssetHandle handle)
{
    auto metadata = GetAssetMetadataImpl(handle).As<EditorAssetMetadata>();
    if (!metadata) return nullptr;

    for (AssetHandle dep : metadata->dependencies)
    {
        if (!loadedAssets.contains(dep))
        {
            LoadAsset(dep);
        }
    }

    if (auto it = loadedAssets.find(handle); it != loadedAssets.end())
    {
        return it->second;
    }

    Scope<AssetLoader> loader = createAssetLoaderFuncs[metadata->type]();
    auto asset = loader->Load(metadata);
    loadedAssets[handle] = asset;

    return asset;
}

Ref<AssetMetadata> EditorAssetManager::GetAssetMetadataImpl(AssetHandle handle)
{
    if (auto it = assetRegistry.find(handle); it == assetRegistry.end())
    {
        Log::Error("Asset Handle: {} doesn't exist!", (uint64_t)handle);
        return nullptr;
    }
    else
        return it->second;
}

void EditorAssetManager::SetAssetDirectoryImpl(const Path &path)
{
    assetDirectory = path;
}

Path EditorAssetManager::GetAssetDirectoryImpl()
{
    return assetDirectory;
}

void EditorAssetManager::RegisterAssetsImpl()
{
    for (auto &dir : std::filesystem::recursive_directory_iterator(assetDirectory))
    {
        if (dir.is_regular_file() && dir.path().extension() == ".meta")
        {
            Path relativeAssetPath = Path(dir.path()).RelativeTo(assetDirectory);
            RegisterAsset(relativeAssetPath);
        }
    }
}

void EditorAssetManager::RegisterAssetImpl(const Path &metadataPath)
{
    Path assetPath = metadataPath;
    assetPath.replace_extension("");
    
    if (registeredFiles.contains(assetPath))
    {
        Log::Warn("Asset Already Registered! Path: {}", metadataPath.string());
        return;
    }

    std::string str = FileSystem::ReadText(assetDirectory / metadataPath);
    YamlDeserializer data(str);

    AssetType assetType = data["Type"].As<AssetType>();
    
    Scope<AssetMetadataSerializer> serializer = createAssetMetadataSerializerFuncs[assetType]();
    auto metadata = serializer->Deserialize(data, assetPath);

    registeredFiles[assetPath] = metadata->handle;
    assetRegistry[metadata->handle] = metadata;
}

AssetHandle EditorAssetManager::ImportAssetImpl(const Path &assetPath)
{
    if (assetPath.extension().string() == ".meta")
    {
        Log::Error("Cannot Import Meta File!");
        return {};
    }
    if (registeredFiles.contains(assetPath.string()))
    {
        Log::Warn("Asset Already Registered! Path: {}", assetPath.string());
        return {};
    }
    AssetType type = AssetType::None;
    if (assetPath.extension().string() == ".png" || assetPath.extension().string() == ".tga" || assetPath.extension().string() == ".jpg")
    {
        type = AssetType::Texture2D;
    }
    else if (assetPath.extension().string() == ".fbx" || assetPath.extension().string() == ".obj" || assetPath.extension().string() == ".gltf" || assetPath.extension().string() == ".pmx")
    {
        type = AssetType::Model;
    }
    else if (assetPath.extension().string() == ".zaf")
    {
        type = AssetType::Scene;
    }
    else if (assetPath.extension().string() == ".mesh")
    {
        type = AssetType::Mesh;
    }
    else
    {
        Log::Error("Unknown asset type!");
    }
    Scope<AssetImporter> importer = createAssetImporterFuncs[type]();
    auto metadata = importer->Import(assetPath);

    assetRegistry[metadata->handle] = metadata;
    registeredFiles[metadata->filePath] = metadata->handle;

    Scope<AssetMetadataSerializer> serializer = createAssetMetadataSerializerFuncs[type]();
    std::string metadataStr = serializer->Serialize(metadata);
    FileSystem::SaveText(assetDirectory / (metadata->filePath.string() + ".meta"), metadataStr);

    return metadata->handle;
}

void EditorAssetManager::LoadBuiltinShaderImpl()
{
    Ref<EditorAssetMetadata> unlitShaderMetadata = CreateRef<EditorAssetMetadata>(unlitShaderHandle, AssetType::Shader, "");
    unlitShaderMetadata->detail.As<ShaderMetadata>().name = "Unlit Shader";

    Ref<ShaderAsset> unlitShader = CreateRef<ShaderAsset>(unlitShaderHandle, "assets/shaders/gbuffer_shader.glsl");

    assetRegistry[unlitShaderHandle] = unlitShaderMetadata;
    loadedAssets[unlitShaderHandle] = unlitShader;
}

AssetHandle EditorAssetManager::GetBuiltinShader(BuiltinShaderType type)
{
    if (type == BuiltinShaderType::Unlit)
    {
        return unlitShaderHandle;
    }
    return AssetHandle{};
}
}
