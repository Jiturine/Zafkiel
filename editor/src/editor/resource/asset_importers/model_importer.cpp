#include "model_importer.h"
#include "core/meta/serializer/binary_serializer.h"
#include "editor/resource/editor_asset_manager.h"
#include "editor/resource/asset_metadata_serializers/material_metadata_serializer.h"
#include <stb_image.h>
#include <stb_image_write.h>

namespace Zafkiel 
{

static AssetHandle defaultShaderHandle = EditorAssetManager::unlitShaderHandle;

AssetHandle ModelImporter::ImportMaterial(const AssimpMaterial &material)
{
    AssetHandle materialHandle;
    
    std::string materialName = material.GetName();
    if (materialName.empty()) 
        materialName = std::format("Unnamed_Material_{}", unnamedMaterialIndex++);

    Path materialFilePath = modelGeneratedDirectory / "materials" / (materialName + ".mat");
    AssetHandle shader = defaultShaderHandle;
    
    MaterialAsset materialAsset(defaultShaderHandle, materialName, ShaderFamily::BlinnPhong);

    // Blinn-Phong 模型 颜色值
    // vec3 color;
    // if (material.GetVec3(AI_MATKEY_COLOR_DIFFUSE, color))
    //     materialData.SetUniform("AlbedoColor", color);
    // else
    //     materialData.SetUniform("AlbedoColor", vec3(1.0f));

    // if (material.GetVec3(AI_MATKEY_COLOR_SPECULAR, color))
    //     materialData.SetUniform("SpecularColor", color);
    // else
    //     materialData.SetUniform("SpecularColor", vec3(0.0f));

    // TODO: 其他 ...

    // 读取纹理
    if (AssimpTexture diffuseTexture; material.GetTexture(aiTextureType_DIFFUSE, diffuseTexture))
    {
        AssetHandle textureHandle = ImportAssimpTexture(diffuseTexture, materialHandle);
        materialAsset.SetTexture2D("DiffuseTexture", textureHandle);
    }
    // if (AssimpTexture specularTexture; material.GetTexture(aiTextureType_SPECULAR, specularTexture))
    // {
    //     AssetHandle textureHandle = ImportAssimpTexture(specularTexture, materialHandle);
    //     materialData.SetTexture2D("SpecularTexture", textureHandle);
    // }

    std::vector<AssetHandle> dependencies;
    for (auto &[paramName, param] : materialAsset.GetParameters())
    {
        if (param.type->GetCategory() == ShaderReflection::ResourceTypeCategory::SampledImage)
        {
            dependencies.push_back(param.handle);
        }
    }

    auto materialMetadata = CreateRef<EditorAssetMetadata>(materialHandle, AssetType::Material, dependencies, materialFilePath);
    materialMetadata->detail.As<MaterialMetadata>().name = materialName;
    
    // 生成meta文件
    MaterialMetadataSerializer metadataSerializer;
    std::string metadataStr = metadataSerializer.Serialize(materialMetadata);
    FileSystem::SaveText(EditorAssetManager::GetAssetDirectory() / (materialFilePath.string() + ".meta"), metadataStr); 

    // 生成.mat文件
    std::string materialStr = materialAsset.Serialize();
    FileSystem::SaveText(EditorAssetManager::GetAssetDirectory() / materialFilePath, materialStr); 
    
    // 注册meta文件 TODO: 考虑是否需要material register
    EditorAssetManager::RegisterAsset(materialFilePath.string() + ".meta");
    
    return materialHandle;
}

AssetHandle ModelImporter::ImportAssimpTexture(const AssimpTexture &texture, AssetHandle materialHandle)
{
    AssetHandle textureHandle;
    if (texture.IsEmbedded()) // 内嵌纹理
    {
        int texIndex = texture.GetIndex();

        if (registeredEmbeddedTextures.contains(texIndex))
        {
            textureHandle = registeredEmbeddedTextures.at(texIndex);
        }
        else
        {
            Path textureFilePath = SaveEmbeddedTexture(texture);
            textureHandle = EditorAssetManager::ImportAsset(textureFilePath);
            registeredEmbeddedTextures[texIndex] = textureHandle;
        }
    }
    else // 外部文件路径
    {
        Path textureFilePath = modelDirectory / texture.GetPath();

        if (EditorAssetManager::IsFileRegisterd(textureFilePath))
        {
            textureHandle = EditorAssetManager::GetRegisterdAsset(textureFilePath);
        }
        else
        {
            textureHandle = EditorAssetManager::ImportAsset(textureFilePath);
        }
    }
    return textureHandle;
}

void ModelImporter::SaveCompressedTexture2DFromMemory(const uint8_t *data, const uint32_t size, const Path &filePath)
{
    int width, height, channels;
    stbi_set_flip_vertically_on_load(1);
    auto rawData = stbi_load_from_memory(data, size, &width, &height, &channels, 0);
    if (!rawData)
    {
        Log::Error("Failed to load Embedded image!");
    }
    stbi_write_png(filePath.string().c_str(), width, height, channels, rawData, 0);
    stbi_image_free(rawData);
}

void ModelImporter::SaveRawTexture2DFromMemory(uint32_t width, uint32_t height, uint32_t channels, const uint8_t *data, const Path &filePath)
{
    stbi_write_png(filePath.string().c_str(), width, height, channels, data, 0);
}

Path ModelImporter::SaveEmbeddedTexture(const AssimpTexture &texture)
{
    Path texturePath = EditorAssetManager::GetAssetDirectory() / modelGeneratedDirectory / "embedded_textures" / std::format("embedded_texture_{}.png", texture.GetIndex());
    if (texture.IsCompressed()) // 压缩纹理，例如 PNG/JPG
    {
        SaveCompressedTexture2DFromMemory(texture.GetData(), texture.GetDataSize(), texturePath);
    }
    else // 原始 RGBA 数据 TODO: 默认ARGB8888 格式，需要转换后才能使用
    {
        SaveRawTexture2DFromMemory(texture.GetWidth(), texture.GetHeight(), 4, texture.GetData(), texturePath);
    }
    return texturePath.RelativeTo(EditorAssetManager::GetAssetDirectory());
}

Path ModelImporter::SaveMesh(uint32_t meshIndex, const aiMesh *mesh)
{
    Path meshPath = modelGeneratedDirectory / "meshes" / std::format("mesh_{}.mesh", meshIndex);
    std::vector<MeshVertex> vertices;
    std::vector<uint32_t> indices;

    for (size_t i = 0; i < mesh->mNumVertices; i++)
    {
        MeshVertex vertex;
        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z; 
        vertex.normal.x = mesh->mNormals[i].x;
        vertex.normal.y = mesh->mNormals[i].y;
        vertex.normal.z = mesh->mNormals[i].z;
        if (mesh->mTextureCoords[0])
        {
            vertex.texCoords.x = mesh->mTextureCoords[0][i].x; 
            vertex.texCoords.y = mesh->mTextureCoords[0][i].y;
        }
        vertices.push_back(vertex);
    }
    for (size_t i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (size_t j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    BinarySerializer serializer;
    serializer.BeginMap();

    serializer.Key("Vertices");
    serializer.WritePack(vertices.data(), vertices.size() * sizeof(MeshVertex));

    serializer.Key("Indices");
    serializer.WritePack(indices.data(), indices.size() * sizeof(uint32_t));

    serializer.EndMap();

    auto output = serializer.GetOutput();
    FileSystem::SaveBytes(EditorAssetManager::GetAssetDirectory() / meshPath, output);

    return meshPath;
}

Ref<EditorAssetMetadata> ModelImporter::Import(const Path &assetPath)
{
    AssimpImporter importer(EditorAssetManager::GetAssetDirectory() / assetPath);
    modelAssetPath = assetPath;
    modelDirectory = modelAssetPath.parent_path();
    modelGeneratedDirectory = modelDirectory / std::format("{}_generated", assetPath.filename().stem().string());

    std::vector<AssetHandle> dependencies;

    for (auto [materialIndex, material] : importer.GetMaterials())
    {
        auto materialHandle = ImportMaterial(material);
        materialMap[materialIndex] = materialHandle;
        dependencies.push_back(materialHandle);
    }

    ModelAssetNode rootNode;
    ImportModelNode(importer.GetRootNode(), rootNode);
    for (auto &[_, mesh] : registeredMeshes)
    {
        dependencies.push_back(mesh);
    }
    
    YamlSerializer serializer;
    serializer.BeginMap();

    serializer.Key("Root");
    SerializeModelNode(rootNode, serializer);
    serializer.EndMap();

    Path modelPath = modelGeneratedDirectory / "model" / std::format("{}.model", assetPath.filename().stem().string());

    std::string modelStr = serializer.c_str();
    FileSystem::SaveText(EditorAssetManager::GetAssetDirectory() / modelPath, modelStr);

    auto metadata = CreateRef<EditorAssetMetadata>(modelHandle, AssetType::Model, dependencies, modelPath);

    return metadata;
}

void ModelImporter::ImportModelNode(const AssimpNode &node, ModelAssetNode &modelAssetNode)
{
    modelAssetNode.localTransform = node.GetLocalTransform();
    modelAssetNode.name = node.GetName();

    for (auto [meshIndex, mesh] : node.GetMeshes())
    {
        AssetHandle meshHandle;
        if (registeredMeshes.contains(meshIndex))
        {
            meshHandle = registeredMeshes[meshIndex];
        }
        else 
        {
            Path meshPath = SaveMesh(meshIndex, mesh);
            meshHandle = EditorAssetManager::ImportAsset(meshPath);
            registeredMeshes[meshIndex] = meshHandle;
        }
        auto materialHandle = materialMap.at(mesh->mMaterialIndex);
        modelAssetNode.meshes.emplace_back(meshHandle, materialHandle);
    }

    for (auto child : node.GetChildren())
    {
        modelAssetNode.children.push_back(ModelAssetNode());
        ImportModelNode(child, modelAssetNode.children.back());
    }
}

void ModelImporter::SerializeModelNode(ModelAssetNode &node, ISerializer &serializer)
{
    serializer.BeginMap();
    serializer.Key("Name").Value(node.name);
    serializer.Key("LocalTransform").Value(node.localTransform);
    serializer.Key("Meshes").BeginSeq();
    for (auto &mesh : node.meshes)
    {
        serializer.BeginMap();
        serializer.Key("MeshHandle").Value(mesh.mesh);
        serializer.Key("MaterialHandle").Value(mesh.material);
        serializer.EndMap();
    }
    serializer.EndSeq();
    serializer.Key("Children").BeginSeq();
    for (auto &child : node.children)
    {
        SerializeModelNode(child, serializer);
    }
    serializer.EndSeq();
    
    serializer.EndMap();
}

}