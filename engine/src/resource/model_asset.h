#pragma once

#include "core/base/memory.h"
#include "function/render/model.h"
#include "resource/asset.h"
#include "resource/material_asset.h"
#include "resource/mesh_asset.h"
#include "resource/asset_manager.h"

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
    // void GenerateModelNode(const ModelAssetNode &assetNode, ModelNode &node);

    void LoadModelNode(ModelAssetNode &node, const IDeserializer &nodeData);

    ModelAssetNode rootNode;
};
    
}