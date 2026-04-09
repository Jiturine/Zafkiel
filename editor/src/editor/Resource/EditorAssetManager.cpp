#include "Core/Base/Utils.h"
#include "editor/Resource/EditorAssetManager.h"
#include "Core/Meta/Serializer/YamlSerializer.h"
#include "Function/Render/Renderer.h"
#include "Function/Scene/Scene.h"
#include "Resource/SceneAsset.h"
#include "Resource/ShaderAsset.h"
#include "Resource/Texture2DAsset.h"
#include "stb_image.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "Platform/Filesystem/Filesystem.h"
#include "Resource/Texture2DAsset.h"
#include "Resource/MaterialAsset.h"

#include "editor/Resource/AssetImporters/MeshImporter.h"
#include "editor/Resource/AssetImporters/ModelImporter.h"
#include "editor/Resource/AssetImporters/Texture2DImporter.h"
#include "editor/Resource/AssetImporters/SceneImporter.h"
#include "editor/Resource/AssetImporters/MaterialImporter.h"
#include "editor/Resource/AssetImporters/FontImporter.h"

#include "editor/Resource/AssetLoaders/MeshLoader.h"
#include "editor/Resource/AssetLoaders/ModelLoader.h"
#include "editor/Resource/AssetLoaders/MaterialLoader.h"
#include "editor/Resource/AssetLoaders/SceneLoader.h"
#include "editor/Resource/AssetLoaders/Texture2DLoader.h"
#include "editor/Resource/AssetLoaders/FontLoader.h"

#include "editor/Resource/AssetMetadataSerializers/MeshMetadataSerializer.h"
#include "editor/Resource/AssetMetadataSerializers/ModelMetadataSerializer.h"
#include "editor/Resource/AssetMetadataSerializers/MaterialMetadataSerializer.h"
#include "editor/Resource/AssetMetadataSerializers/SceneMetadataSerializer.h"
#include "editor/Resource/AssetMetadataSerializers/Texture2DMetadataSerializer.h"
#include "editor/Resource/AssetMetadataSerializers/FontMetadataSerializer.h"

namespace Zafkiel
{
    
static std::unordered_map<AssetType, std::function<Scope<AssetLoader>()>> createAssetLoaderFuncs
{
    {AssetType::Material, [](){ return CreateScope<MaterialLoader>(); } },
    {AssetType::Model, [](){ return CreateScope<ModelLoader>(); } },
    {AssetType::Mesh, [](){ return CreateScope<MeshLoader>(); } },
    {AssetType::Scene, [](){ return CreateScope<SceneLoader>(); } },
    {AssetType::Texture2D, [](){ return CreateScope<Texture2DLoader>(); } },
    {AssetType::Font, [](){ return CreateScope<Texture2DLoader>(); } },
};

static std::unordered_map<AssetType, std::function<Scope<AssetMetadataSerializer>()>> createAssetMetadataSerializerFuncs
{
    {AssetType::Material, [](){ return CreateScope<MaterialMetadataSerializer>(); } },
    {AssetType::Model, [](){ return CreateScope<ModelMetadataSerializer>(); } },
    {AssetType::Mesh, [](){ return CreateScope<MeshMetadataSerializer>(); } },
    {AssetType::Scene, [](){ return CreateScope<SceneMetadataSerializer>(); } },
    {AssetType::Texture2D, [](){ return CreateScope<Texture2DMetadataSerializer>(); } },
    {AssetType::Font, [](){ return CreateScope<FontMetadataSerializer>(); } },
};

bool EditorAssetManager::IsAssetValid(AssetHandle handle) const
{
    return assetRegistry.contains(handle);
}
bool EditorAssetManager::IsAssetLoaded(AssetHandle handle) const
{
    return loadedAssets.contains(handle);
}
bool EditorAssetManager::IsFileRegisterd(const Path &path) const
{
    return registeredFiles.contains(path);
}
AssetHandle EditorAssetManager::GetRegisterdAsset(const Path &assetPath) const
{
    return registeredFiles.at(assetPath);
}

// 尽量延迟加载依赖项
Ref<Asset> EditorAssetManager::GetAsset(AssetHandle handle)
{
    auto metadata = GetAssetMetadata(handle).As<EditorAssetMetadata>();
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
Ref<Asset> EditorAssetManager::LoadAsset(AssetHandle handle)
{
    auto metadata = GetAssetMetadata(handle).As<EditorAssetMetadata>();
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

Ref<AssetMetadata> EditorAssetManager::GetAssetMetadata(AssetHandle handle)
{
    if (auto it = assetRegistry.find(handle); it == assetRegistry.end())
    {
        Log::Error("Asset Handle: {} doesn't exist!", (uint64_t)handle);
        return nullptr;
    }
    else
        return it->second;
}

void EditorAssetManager::SetAssetDirectory(const Path &path)
{
    assetDirectory = path;
}

Path EditorAssetManager::GetAssetDirectory()
{
    return assetDirectory;
}

void EditorAssetManager::RegisterAssets()
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

void EditorAssetManager::RegisterAsset(const Path &metadataPath)
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

AssetHandle EditorAssetManager::ImportAsset(const Path &assetPath)
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
    std::string ext = assetPath.extension().string();
    if (ext == ".png" || ext == ".tga" || ext == ".jpg")
    {
        return ImportTexture2D(assetPath);
    }
    else if (ext == ".fbx" || ext == ".obj" || ext == ".gltf" || ext == ".pmx")
    {
        return ImportModel(assetPath);
    }
    else if (ext == ".zaf")
    {
        return ImportScene(assetPath);
    }
    else if (ext == ".mesh")
    {
        return ImportMesh(assetPath);
    }
    else if (ext == ".ttf")
    {
        return ImportFont(assetPath);
    }
    else
    {
        Log::Error("Unknown asset type!");
        return {};
    }
}

AssetHandle EditorAssetManager::ImportMaterial(const Path &path)
{
    MaterialImporter importer;
    auto metadata = importer.Import(path);

    assetRegistry[metadata->handle] = metadata;
    registeredFiles[metadata->filePath] = metadata->handle;

    Scope<AssetMetadataSerializer> serializer = createAssetMetadataSerializerFuncs[AssetType::Material]();
    std::string metadataStr = serializer->Serialize(metadata);
    FileSystem::SaveText(assetDirectory / (metadata->filePath.string() + ".meta"), metadataStr);

    return metadata->handle;
}

AssetHandle EditorAssetManager::ImportModel(const Path &path, ShaderFamily shaderFamily)
{
    ModelImporter importer;
    auto metadata = importer.Import(path, shaderFamily);

    assetRegistry[metadata->handle] = metadata;
    registeredFiles[metadata->filePath] = metadata->handle;

    Scope<AssetMetadataSerializer> serializer = createAssetMetadataSerializerFuncs[AssetType::Model]();
    std::string metadataStr = serializer->Serialize(metadata);
    FileSystem::SaveText(assetDirectory / (metadata->filePath.string() + ".meta"), metadataStr);

    return metadata->handle;
}

AssetHandle EditorAssetManager::ImportMesh(const Path &path)
{
    MeshImporter importer;
    auto metadata = importer.Import(path);

    assetRegistry[metadata->handle] = metadata;
    registeredFiles[metadata->filePath] = metadata->handle;

    Scope<AssetMetadataSerializer> serializer = createAssetMetadataSerializerFuncs[AssetType::Mesh]();
    std::string metadataStr = serializer->Serialize(metadata);
    FileSystem::SaveText(assetDirectory / (metadata->filePath.string() + ".meta"), metadataStr);

    return metadata->handle;
}

AssetHandle EditorAssetManager::ImportFont(const Path &path)
{
    FontImporter importer;
    auto metadata = importer.Import(path);

    assetRegistry[metadata->handle] = metadata;
    registeredFiles[metadata->filePath] = metadata->handle;

    Scope<AssetMetadataSerializer> serializer = createAssetMetadataSerializerFuncs[AssetType::Font]();
    std::string metadataStr = serializer->Serialize(metadata);
    FileSystem::SaveText(assetDirectory / (metadata->filePath.string() + ".meta"), metadataStr);

    return metadata->handle;
}

AssetHandle EditorAssetManager::ImportScene(const Path &path)
{
    SceneImporter importer;
    auto metadata = importer.Import(path);

    assetRegistry[metadata->handle] = metadata;
    registeredFiles[metadata->filePath] = metadata->handle;

    Scope<AssetMetadataSerializer> serializer = createAssetMetadataSerializerFuncs[AssetType::Scene]();
    std::string metadataStr = serializer->Serialize(metadata);
    FileSystem::SaveText(assetDirectory / (metadata->filePath.string() + ".meta"), metadataStr);

    return metadata->handle;
}

AssetHandle EditorAssetManager::ImportTexture2D(const Path &path, ImageFormat format)
{
    Texture2DImporter importer;
    auto metadata = importer.Import(path, format);

    assetRegistry[metadata->handle] = metadata;
    registeredFiles[metadata->filePath] = metadata->handle;

    Scope<AssetMetadataSerializer> serializer = createAssetMetadataSerializerFuncs[AssetType::Texture2D]();
    std::string metadataStr = serializer->Serialize(metadata);
    FileSystem::SaveText(assetDirectory / (metadata->filePath.string() + ".meta"), metadataStr);

    return metadata->handle;
}

}
