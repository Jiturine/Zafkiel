#pragma once
#include "editor/resource/asset_importers/asset_importer.h"
#include "editor/resource/assimp_importer.h"
#include "resource/model_asset.h"

namespace Zafkiel 
{

class ModelImporter final : public AssetImporter
{
  public:
    virtual Ref<EditorAssetMetadata> Import(const Path &assetPath) override;

  private:
    AssetHandle ImportMaterial(const AssimpMaterial &material);
    // ModelNodeMetadata ImportModelNode(const AssimpNode &node);
    AssetHandle ImportAssimpTexture(const AssimpTexture &texture, AssetHandle materialHandle);
    void SaveCompressedTexture2DFromMemory(const uint8_t *data, const uint32_t size, const Path &filePath);
    void SaveRawTexture2DFromMemory(uint32_t width, uint32_t height, uint32_t channels, const uint8_t *data, const Path &filePath);
    Path SaveEmbeddedTexture(const AssimpTexture &texture);
    Path SaveMesh(uint32_t meshIndex, const aiMesh *mesh);
    void ImportModelNode(const AssimpNode &node, ModelAssetNode &modelAssetNode);
    void SerializeModelNode(ModelAssetNode &node, ISerializer &serializer);
    
    std::unordered_map<uint32_t, AssetHandle> materialMap;
    std::unordered_map<std::string, AssetHandle> registeredEmbeddedTextures;
    std::unordered_map<uint32_t, AssetHandle> registeredMeshes;
    AssetHandle modelHandle;
    Path modelAssetPath;
    Path modelDirectory;
    Path modelGeneratedDirectory;
    uint32_t unnamedMaterialIndex;
};


}