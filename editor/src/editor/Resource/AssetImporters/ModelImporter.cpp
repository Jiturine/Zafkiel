#include "Core/Meta/Serializer/BinarySerializer.h"
#include "editor/Resource/AssetImporters/ModelImporter.h"
#include "editor/Resource/EditorAssetManager.h"
#include "editor/Resource/AssetMetadataSerializers/MaterialMetadataSerializer.h"
#include <stb_image.h>
#include <stb_image_write.h>

namespace Zafkiel 
{

AssetHandle ModelImporter::ImportMaterial(const AssimpMaterial &material, ShaderFamily shaderFamily)
{
    AssetHandle materialHandle;
    
    std::string materialName = material.GetName();
    if (materialName.empty()) 
        materialName = std::format("Unnamed_Material_{}", unnamedMaterialIndex++);

    Path materialFilePath = modelGeneratedDirectory / "materials" / (materialName + ".mat");
    
    MaterialAsset materialAsset(materialHandle, materialName, shaderFamily);

    if (shaderFamily == ShaderFamily::BlinnPhong)
    {
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

        // 读取纹理
        if (AssimpTexture diffuseTexture; material.GetTexture(aiTextureType_DIFFUSE, diffuseTexture))
        {
            // albedo 纹理是sRGB的，其他不是
            AssetHandle textureHandle = ImportAssimpTexture(diffuseTexture, ImageFormat::RGBA8_sRGB);
            materialAsset.SetTexture2D("DiffuseTexture", textureHandle);
        }
        // if (AssimpTexture specularTexture; material.GetTexture(aiTextureType_SPECULAR, specularTexture))
        // {
        //     AssetHandle textureHandle = ImportAssimpTexture(specularTexture, materialHandle);
        //     materialData.SetTexture2D("SpecularTexture", textureHandle);
        // }
    }
    else if (shaderFamily == ShaderFamily::PBR)
    {
        if (AssimpTexture diffuseTexture; material.GetTexture(aiTextureType_DIFFUSE, diffuseTexture))
        {
            // albedo 纹理是sRGB的，其他不是
            AssetHandle textureHandle = ImportAssimpTexture(diffuseTexture, ImageFormat::RGBA8_sRGB);
            materialAsset.SetTexture2D("DiffuseTexture", textureHandle);
        }
        if (AssimpTexture normalTexture; material.GetTexture(aiTextureType_NORMALS, normalTexture))
        {
            AssetHandle textureHandle = ImportAssimpTexture(normalTexture, ImageFormat::RGBA8);
            materialAsset.SetTexture2D("NormalTexture", textureHandle);
        }
        // gltf
        if (AssimpTexture texture; material.GetTexture(aiTextureType_GLTF_METALLIC_ROUGHNESS, texture))
        {
            auto metalnessRoughness = ImportGltfMetalnessRoughtnessTexture(texture);
            materialAsset.SetTexture2D("MetalnessTexture", metalnessRoughness.metalnessTexture);
            materialAsset.SetTexture2D("RoughnessTexture", metalnessRoughness.roughnessTexture);
        }
        else 
        {
            if (AssimpTexture metalnessTexture; material.GetTexture(aiTextureType_METALNESS, metalnessTexture))
            {
                AssetHandle textureHandle = ImportAssimpTexture(metalnessTexture, ImageFormat::R8);
                materialAsset.SetTexture2D("MetalnessTexture", textureHandle);
            }
            if (AssimpTexture roughnessTexture; material.GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, roughnessTexture))
            {
                AssetHandle textureHandle = ImportAssimpTexture(roughnessTexture, ImageFormat::R8);
                materialAsset.SetTexture2D("RoughnessTexture", textureHandle);
            }
        }
    }

    std::vector<AssetHandle> dependencies;
    for (auto &[paramName, param] : materialAsset.GetParameters())
    {
        if (param.type == MaterialAssetParameterType::SampledImage)
        {
            dependencies.push_back(param.assetHandle);
        }
    }

    auto materialMetadata = CreateRef<EditorAssetMetadata>(materialHandle, AssetType::Material, dependencies, materialFilePath);
    materialMetadata->detail.As<MaterialMetadata>().name = materialName;
    
    // 生成meta文件
    MaterialMetadataSerializer metadataSerializer;
    std::string metadataStr = metadataSerializer.Serialize(materialMetadata);
    FileSystem::SaveText(EditorAssetManager::Instance().GetAssetDirectory() / (materialFilePath.string() + ".meta"), metadataStr); 

    // 生成.mat文件
    std::string materialStr = materialAsset.Serialize();
    FileSystem::SaveText(EditorAssetManager::Instance().GetAssetDirectory() / materialFilePath, materialStr); 
    
    // 注册meta文件 TODO: 考虑是否需要material register
    EditorAssetManager::Instance().RegisterAsset(materialFilePath.string() + ".meta");
    
    return materialHandle;
}

AssetHandle ModelImporter::ImportAssimpTexture(const AssimpTexture &texture, ImageFormat format)
{
    AssetHandle textureHandle;
    if (texture.IsEmbedded()) // 内嵌纹理
    {
        if (registeredEmbeddedTextures.contains(texture.GetPath().string()))
        {
            textureHandle = registeredEmbeddedTextures.at(texture.GetPath().string());
        }
        else
        {
            Path textureFilePath = SaveEmbeddedTexture(texture);
            textureHandle = EditorAssetManager::Instance().ImportTexture2D(textureFilePath, format);
            registeredEmbeddedTextures[texture.GetPath().string()] = textureHandle;
        }
    }
    else // 外部文件路径
    {
        Path textureFilePath = modelDirectory / texture.GetPath();

        if (EditorAssetManager::Instance().IsFileRegisterd(textureFilePath))
        {
            textureHandle = EditorAssetManager::Instance().GetRegisterdAsset(textureFilePath);
        }
        else
        {
            textureHandle = EditorAssetManager::Instance().ImportTexture2D(textureFilePath, format);
        }
    }
    return textureHandle;
}

GltfMetalnessRoughness ModelImporter::ImportGltfMetalnessRoughtnessTexture(const AssimpTexture &texture)
{
    GltfMetalnessRoughness metalnessRoughness;
    std::string textureKey = texture.GetPath().string();

    if (registeredGltfMetalnessRoughnessTextures.contains(textureKey))
    {
        metalnessRoughness = registeredGltfMetalnessRoughnessTextures.at(textureKey);
    }
    else
    {
        auto texturePaths= SaveSeparateMetalnessRoughnessTexture(texture);

        AssetHandle metalnessHandle = EditorAssetManager::Instance().ImportTexture2D(texturePaths.metalnessTexturePath, ImageFormat::R8);
        AssetHandle roughnessHandle = EditorAssetManager::Instance().ImportTexture2D(texturePaths.roughnessTexturePath, ImageFormat::R8);

        metalnessRoughness = {metalnessHandle, roughnessHandle};

        registeredGltfMetalnessRoughnessTextures[textureKey] = metalnessRoughness;
    }
    return metalnessRoughness;
}

Path ModelImporter::SaveEmbeddedTexture(const AssimpTexture &texture)
{
    Path texturePath = EditorAssetManager::Instance().GetAssetDirectory() / modelGeneratedDirectory / "embedded_textures" / std::format("embedded_texture_{}.png", texture.GetPath().string());
    if (texture.IsCompressed()) // 压缩纹理，例如 PNG/JPG
    {
        int width, height, channels;
        stbi_set_flip_vertically_on_load(1);
        auto rawData = stbi_load_from_memory(texture.GetData(), texture.GetDataSize(), &width, &height, &channels, 0);
        if (!rawData)
        {
            Log::Error("Failed to load Embedded image!");
        }
        stbi_write_png(texturePath.string().c_str(), width, height, channels, rawData, 0);
        stbi_image_free(rawData);
    }
    else // 原始 RGBA 数据 TODO: 默认ARGB8888 格式，需要转换后才能使用
    {
        stbi_write_png(texturePath.string().c_str(), texture.GetWidth(), texture.GetHeight(), 4, texture.GetData(), 0);
    }
    return texturePath.RelativeTo(EditorAssetManager::Instance().GetAssetDirectory());
}

GltfMetalnessRoughnessPaths ModelImporter::SaveSeparateMetalnessRoughnessTexture(const AssimpTexture &texture)
{
    Path saveDir = EditorAssetManager::Instance().GetAssetDirectory() / modelGeneratedDirectory / "generated_textures";
    std::filesystem::create_directories(saveDir);

    Path metalnessTexturePath = saveDir / std::format("metalness_texture_{}.png", texture.GetPath().filename().stem().string());
    Path roughnessTexturePath = saveDir / std::format("roughness_texture_{}.png", texture.GetPath().filename().stem().string());

    int width, height, channels;
    stbi_set_flip_vertically_on_load(0);
    unsigned char* rawData = nullptr;

    if (texture.IsEmbedded())
    {
        // 内嵌纹理：从内存加载
        rawData = stbi_load_from_memory(texture.GetData(), texture.GetDataSize(), &width, &height, &channels, 4);
    }
    else
    {
        // 外部文件：从文件路径加载
        Path textureFilePath = EditorAssetManager::Instance().GetAssetDirectory() / modelDirectory / texture.GetPath();
        rawData = stbi_load(textureFilePath.string().c_str(), &width, &height, &channels, 4);
    }

    if (!rawData)
    {
        Log::Error("Failed to load MetallicRoughness texture!");
        return {metalnessTexturePath.RelativeTo(EditorAssetManager::Instance().GetAssetDirectory()),
                roughnessTexturePath.RelativeTo(EditorAssetManager::Instance().GetAssetDirectory())};
    }

    std::vector<uint8> metalnessData(width * height);
    std::vector<uint8> roughnessData(width * height);

    // B通道是metalness，G通道是roughness
    for (int i = 0; i < width * height; i++)
    {
        metalnessData[i] = rawData[i * 4 + 2];  // B通道
        roughnessData[i] = rawData[i * 4 + 1];  // G通道
    }

    // 保存为单通道PNG
    stbi_write_png(metalnessTexturePath.string().c_str(), width, height, 1, metalnessData.data(), 0);
    stbi_write_png(roughnessTexturePath.string().c_str(), width, height, 1, roughnessData.data(), 0);

    stbi_image_free(rawData);

    return {metalnessTexturePath.RelativeTo(EditorAssetManager::Instance().GetAssetDirectory()),
            roughnessTexturePath.RelativeTo(EditorAssetManager::Instance().GetAssetDirectory())};
}

Path ModelImporter::SaveMesh(uint32 meshIndex, const aiMesh *mesh)
{
    Path meshPath = modelGeneratedDirectory / "meshes" / std::format("mesh_{}.mesh", meshIndex);
    std::vector<MeshVertex> vertices;
    std::vector<uint32> indices;

    for (size_t i = 0; i < mesh->mNumVertices; i++)
    {
        MeshVertex vertex;
        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z; 
        vertex.normal.x = mesh->mNormals[i].x;
        vertex.normal.y = mesh->mNormals[i].y;
        vertex.normal.z = mesh->mNormals[i].z;
        vertex.tangent.x = mesh->mTangents[i].x;
        vertex.tangent.y = mesh->mTangents[i].y;
        vertex.tangent.z = mesh->mTangents[i].z;
        vec3 bitangent(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
        float handedness = (vec3::dot(vec3::cross(vertex.normal, vertex.tangent), bitangent) < 0.0f) ? -1.0f : 1.0f;
        vertex.tangent.w = handedness;
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
    serializer.WritePack(indices.data(), indices.size() * sizeof(uint32));

    serializer.EndMap();

    auto output = serializer.GetOutput();
    FileSystem::SaveBytes(EditorAssetManager::Instance().GetAssetDirectory() / meshPath, output);

    return meshPath;
}

Ref<EditorAssetMetadata> ModelImporter::Import(const Path &assetPath, ShaderFamily shaderFamily)
{
    AssimpImporter importer(EditorAssetManager::Instance().GetAssetDirectory() / assetPath);
    modelAssetPath = assetPath;
    modelDirectory = modelAssetPath.parent_path();
    modelGeneratedDirectory = modelDirectory / std::format("{}_generated", assetPath.filename().stem().string());

    std::vector<AssetHandle> dependencies;

    for (auto [materialIndex, material] : importer.GetMaterials())
    {
        auto materialHandle = ImportMaterial(material, shaderFamily);
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
    FileSystem::SaveText(EditorAssetManager::Instance().GetAssetDirectory() / modelPath, modelStr);

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
            meshHandle = EditorAssetManager::Instance().ImportAsset(meshPath);
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
