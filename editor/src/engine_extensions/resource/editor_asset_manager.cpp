#include "core/meta/serializer/deserialize.h"
#include "editor_asset_manager.h"
#include "function/render/model.h"
#include "stb_image.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "engine.h"
#include "platform/filesystem/filesystem.h"

namespace Zafkiel
{
EditorAssetManager::EditorAssetManager() {}

bool EditorAssetManager::IsAssetValid(AssetHandle handle) const
{
    return assetRegistry.metadatas.contains(handle);
}
bool EditorAssetManager::IsAssetLoaded(AssetHandle handle) const
{
    return loadedAssets.contains(handle);
}

Ref<Asset> EditorAssetManager::GetAsset(AssetHandle handle)
{
    if (!assetRegistry.metadatas.contains(handle) && !assetRegistry.parentMap.contains(handle)) return nullptr;

    Ref<Asset> asset;
    if (IsAssetLoaded(handle))
    {
        asset = loadedAssets.at(handle);
    }
    else
    {
        Ref<AssetMetadata> metadata = assetRegistry.metadatas[handle];
        asset = LoadAsset(handle, metadata);
        if (!asset)
        {
            Log::CoreError("Load Asset failed!");
        }
        loadedAssets[handle] = asset;
    }
    return asset;
}

void EditorAssetManager::SetAssetDirectory(const Path &path)
{
    assetDirectory = path;
}

Path EditorAssetManager::GetAssetDirectory()
{
    return assetDirectory;
}
void EditorAssetManager::SerializeAssetRegistry()
{
    const auto &str = Serialize(assetRegistry);
    FileSystem::SaveText(assetDirectory / "asset_registry.yaml", str);
}

void EditorAssetManager::DeserializeAssetRegistry()
{
    const std::string str = FileSystem::ReadText(assetDirectory / "asset_registry.yaml");
    assetRegistry = Deserialize<AssetRegistry>(str);
}

AssetHandle EditorAssetManager::ImportAsset(const Path &assetPath)
{
    AssetHandle handle;
    if (assetPath.extension().string() == ".png")
    {
        ImportTexture2D(assetPath);
    }
    else if (assetPath.extension().string() == ".fbx" || assetPath.extension().string() == ".obj" || assetPath.extension().string() == ".gltf")
    {
        ImportModel(assetPath);
    }
    else
    {
        Log::CoreError("Unknown asset type!");
    }
    return handle;
}

AssetHandle EditorAssetManager::ImportTexture2D(const Path &assetPath)
{
    AssetHandle handle;
    auto metadata = MakeRef<Texture2DMetadata>();
    metadata->type = AssetType::Texture2D;
    metadata->filePath = assetPath;
    assetRegistry.metadatas[handle] = metadata;
    return handle;
}

static mat4 AiMatrixToMat(const aiMatrix4x4 &m)
{
    mat4 out;
    // clang-format off
    out[0][0] = m.a1; out[1][0] = m.a2; out[2][0] = m.a3; out[3][0] = m.a4;
    out[0][1] = m.b1; out[1][1] = m.b2; out[2][1] = m.b3; out[3][1] = m.b4;
    out[0][2] = m.c1; out[1][2] = m.c2; out[2][2] = m.c3; out[3][2] = m.c4;
    out[0][3] = m.d1; out[1][3] = m.d2; out[2][3] = m.d3; out[3][3] = m.d4;
    // clang-format on
    return out;
}

static std::pair<Ref<Mesh>, Ref<MeshMetadata>> ImportMesh(aiMesh *mesh, const aiScene *scene, const mat4 parentTransform, Path nodePath)
{
    Ref<MeshMetadata> meshMetadata = MakeRef<MeshMetadata>();
    meshMetadata->type = AssetType::Mesh;
    meshMetadata->transform = parentTransform;
    const char *mName = mesh->mName.C_Str();
    std::string meshName = (strlen(mName) > 0) ? mName : "Unnamed";
    meshMetadata->nodePath = nodePath / meshName;

    std::vector<MeshVertex> vertices;
    std::vector<uint32_t> indices;
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        MeshVertex vertex;
        vertex.position = vec4(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z, 1.0f);
        if (mesh->HasNormals())
        {
            vertex.normal = vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        }
        if (mesh->HasTextureCoords(0))
        {
            vertex.texCoords = vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        }
        vertices.push_back(vertex);
    }
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    Ref<Mesh> meshAsset = Engine::GetGraphicsContext()->CreateMesh(vertices, indices);
    return {meshAsset, meshMetadata};
}

static void ImportNode(aiNode *node, const aiScene *scene, const mat4 &parentTransform, std::vector<Ref<Mesh>> &meshes, Ref<ModelMetadata> &modelMetadata, Path nodePath)
{
    mat4 local = AiMatrixToMat(node->mTransformation);
    mat4 global = parentTransform * local;

    // 处理节点所有的网格（如果有的话）
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        auto [meshAsset, meshMetadata] = ImportMesh(mesh, scene, global, nodePath);
        meshes.push_back(meshAsset);
        modelMetadata->meshes[AssetHandle{}] = meshMetadata;
    }
    // 接下来对它的子节点重复这一过程
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ImportNode(node->mChildren[i], scene, global, meshes, modelMetadata, nodePath / node->mName.C_Str());
    }
}

AssetHandle EditorAssetManager::ImportModel(const Path &assetPath)
{
    AssetHandle handle;
    auto metadata = MakeRef<ModelMetadata>();
    metadata->type = AssetType::Model;
    metadata->filePath = assetPath;

    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(assetDirectory / assetPath, aiProcess_Triangulate | aiProcess_GenSmoothNormals);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        Log::CoreError("Failed to load mesh: {}", importer.GetErrorString());
        return AssetHandle{};
    }
    std::vector<Ref<Mesh>> meshes;
    Ref<Model> model = MakeRef<Model>(meshes);
    ImportNode(scene->mRootNode, scene, mat4(1.0f), meshes, metadata, Path());
    for (auto [meshHandle, meshMetadata] : metadata->meshes)
    {
        meshMetadata->parentModel = handle;
        assetRegistry.parentMap[meshHandle] = handle;
    }

    assetRegistry.metadatas[handle] = metadata;
    loadedAssets[handle] = model;
    return handle;
}

Ref<Asset> EditorAssetManager::LoadAsset(AssetHandle handle, const Ref<AssetMetadata> &metadata)
{
    switch (metadata->type)
    {
        using enum AssetType;
    case Texture2D:
        return LoadTexture2D(metadata.As<Texture2DMetadata>());
    case Model:
        return LoadModel(metadata.As<ModelMetadata>());
    case Mesh: {
        auto modelMetadata = assetRegistry.metadatas[metadata.As<MeshMetadata>()->parentModel].As<ModelMetadata>();
        LoadModel(modelMetadata);
        return GetAsset(handle);
    }

    default:
        Log::CoreError("Unknown data type!");
        break;
    }
    return nullptr;
}

Ref<Asset> EditorAssetManager::LoadTexture2D(const Ref<Texture2DMetadata> &metadata) const
{
    int width, height, channels;
    stbi_set_flip_vertically_on_load(1);
    auto data = stbi_load(metadata->filePath.string().c_str(), &width, &height, &channels, 0);
    if (!data)
    {
        Log::CoreError("Failed to load image! Path: {}", metadata->filePath.string());
    }
    TextureSpecification spec;
    spec.width = width;
    spec.height = height;

    if (channels == 4)
    {
        spec.format = TextureFormat::RGBA8;
    }
    else if (channels == 3)
    {
        spec.format = TextureFormat::RGB8;
    }
    else
    {
        Log::CoreError("Format not supported!");
    }

    size_t size = width * height * channels;
    Ref<Asset> texture = Engine::GetGraphicsContext()->CreateTexture2D(spec, Buffer(data, size));

    stbi_image_free(data);

    return texture;
}

static Ref<Mesh> LoadMesh(aiMesh *mesh, const aiScene *scene)
{
    std::vector<MeshVertex> vertices;
    std::vector<uint32_t> indices;
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        MeshVertex vertex;
        vertex.position = vec4(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z, 1.0f);
        if (mesh->HasNormals())
        {
            vertex.normal = vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        }
        if (mesh->HasTextureCoords(0))
        {
            vertex.texCoords = vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        }
        vertices.push_back(vertex);
    }
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    Ref<Mesh> meshAsset = Engine::GetGraphicsContext()->CreateMesh(vertices, indices);
    return meshAsset;
}

void EditorAssetManager::LoadNode(aiNode *node, const aiScene *scene, const mat4 &parentTransform, std::vector<Ref<Mesh>> &meshes, Path nodePath, Ref<ModelMetadata> modelMetadata)
{
    mat4 local = AiMatrixToMat(node->mTransformation);
    mat4 global = parentTransform * local;
    // 处理节点所有的网格（如果有的话）
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        auto meshAsset = LoadMesh(mesh, scene);

        const char *mName = mesh->mName.C_Str();
        std::string meshName = (strlen(mName) > 0) ? mName : "Unnamed";
        std::string fullPath = nodePath / meshName;

        if (auto it = std::find_if(modelMetadata->meshes.begin(), modelMetadata->meshes.end(),
                [&fullPath](std::pair<const AssetHandle, const Ref<MeshMetadata> &> kvp) {
                    return kvp.second->nodePath == fullPath;
                });
            it != modelMetadata->meshes.end())
        {
            meshAsset->handle = it->first;
            assetRegistry.metadatas[it->first].As<MeshMetadata>()->transform = global;
            loadedAssets[it->first] = meshAsset;
            meshes.push_back(meshAsset);
        }
        else
        {
            Log::CoreError("Unknown Mesh Path: {}", fullPath);
        }
    }
    // 接下来对它的子节点重复这一过程
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        LoadNode(node->mChildren[i], scene, global, meshes, nodePath / node->mName.C_Str(), modelMetadata);
    }
}

Ref<Asset> EditorAssetManager::LoadModel(const Ref<ModelMetadata> &metadata)
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(assetDirectory / metadata->filePath, aiProcess_Triangulate | aiProcess_GenSmoothNormals);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        Log::CoreError("Failed to load mesh: {}", importer.GetErrorString());
        return nullptr;
    }
    std::vector<Ref<Mesh>> meshes;
    LoadNode(scene->mRootNode, scene, mat4(1.0f), meshes, Path(), metadata);
    Ref<Model> model = MakeRef<Model>(meshes);
    return model;
}

}