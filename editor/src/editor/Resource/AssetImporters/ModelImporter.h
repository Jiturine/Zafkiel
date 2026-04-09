#pragma once
#include "editor/Resource/EditorAsset.h"
#include "editor/Resource/AssimpImporter.h"
#include "Resource/ModelAsset.h"

namespace Zafkiel 
{

struct GltfMetalnessRoughness
{
    AssetHandle metalnessTexture;
    AssetHandle roughnessTexture;
};

struct GltfMetalnessRoughnessPaths
{
    Path metalnessTexturePath;
    Path roughnessTexturePath;
};

class ModelImporter
{
  public:
    Ref<EditorAssetMetadata> Import(const Path &assetPath, ShaderFamily shaderFamily = ShaderFamily::BlinnPhong);

  private:
    AssetHandle ImportMaterial(const AssimpMaterial &material, ShaderFamily shaderFamily);
    AssetHandle ImportAssimpTexture(const AssimpTexture &texture, ImageFormat format);
    GltfMetalnessRoughness ImportGltfMetalnessRoughtnessTexture(const AssimpTexture &texture);
    Path SaveEmbeddedTexture(const AssimpTexture &texture);
    GltfMetalnessRoughnessPaths SaveSeparateMetalnessRoughnessTexture(const AssimpTexture &texture);
    Path SaveMesh(uint32 meshIndex, const aiMesh *mesh);
    void ImportModelNode(const AssimpNode &node, ModelAssetNode &modelAssetNode);
    void SerializeModelNode(ModelAssetNode &node, ISerializer &serializer);
    
    std::unordered_map<uint32, AssetHandle> materialMap;
    std::unordered_map<std::string, AssetHandle> registeredEmbeddedTextures;
    std::unordered_map<std::string, GltfMetalnessRoughness> registeredGltfMetalnessRoughnessTextures;
    std::unordered_map<uint32, AssetHandle> registeredMeshes;
    AssetHandle modelHandle;
    Path modelAssetPath;
    Path modelDirectory;
    Path modelGeneratedDirectory;
    uint32 unnamedMaterialIndex;
};


}