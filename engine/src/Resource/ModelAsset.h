#pragma once

#include "Core/Base/Memory.h"
#include "Function/Render/Model.h"
#include "Resource/Asset.h"
#include "Resource/MaterialAsset.h"
#include "Resource/MeshAsset.h"
#include "Resource/AssetManager.h"

namespace Zafkiel
{
struct ModelMeshAsset
{
    AssetHandle mesh;
    AssetHandle material;
};

struct ModelAssetNode
{
    std::string name;
    mat4 localTransform;
    std::vector<ModelMeshAsset> meshes;
    std::vector<ModelAssetNode> children;
};

class ModelAsset : public Asset
{
  public:
    virtual AssetType GetAssetType() const override { return AssetType::Model; }

    ModelAsset(AssetHandle handle, const std::string &data);
    ModelAsset(AssetHandle handle, ModelAssetNode node);

    const ModelAssetNode &GetRootNode() const { return rootNode; }
    ModelAssetNode &GetRootNode() { return rootNode; }

  private:
    void LoadModelNode(ModelAssetNode &node, const IDeserializer &nodeData);

    ModelAssetNode rootNode;
};
    
}