#include "model_asset.h"
#include "core/meta/serializer/yaml_serializer.h"

namespace Zafkiel 
{

ModelAsset::ModelAsset(AssetHandle handle, const std::string &data) 
    : Asset(handle)
{
    YamlDeserializer modelData(data);

    auto &rootNodeData = modelData["Root"];

    LoadModelNode(rootNode, rootNodeData);


    // Renderer::Submit([self = Ref(this)]() mutable {
    //     ModelNode modelRootNode;
    //     self->GenerateModelNode(self->rootNode, modelRootNode); 
    //     self->model = CreateScope<Model>(std::move(modelRootNode));
    // });
}

ModelAsset::ModelAsset(AssetHandle handle, ModelAssetNode node)
    : Asset(handle), rootNode(node)
{
    // ModelNode modelRootNode;
    // GenerateModelNode(rootNode, modelRootNode);
    
    // model = CreateScope<Model>(std::move(modelRootNode));
}

void ModelAsset::LoadModelNode(ModelAssetNode &node, const IDeserializer &nodeData)
{
    node.localTransform = nodeData["LocalTransform"].As<mat4>();
    node.name = nodeData["Name"].As<std::string>();

    for (auto mesh : nodeData["Meshes"].SeqElems())
    {
        node.meshes.emplace_back(mesh["MeshHandle"].As<AssetHandle>(), mesh["MaterialHandle"].As<AssetHandle>());
    }
    for (auto childData : nodeData["Children"].SeqElems())
    {
        node.children.push_back(ModelAssetNode());
        LoadModelNode(node.children.back(), childData);
    }
}

// void ModelAsset::GenerateModelNode(const ModelAssetNode &assetNode, ModelNode &node)
// {
//     node.name = assetNode.name;
//     node.localTransform = assetNode.localTransform;
//     for (auto &modelMesh : assetNode.meshes)
//     {
//         node.meshes.emplace_back(AssetManager::GetAsset(modelMesh.mesh).As<MeshAsset>()->GetMesh(),
//                                  AssetManager::GetAsset(modelMesh.material).As<MaterialAsset>()->GetMaterial());
//     }
//     for (auto &child : assetNode.children)
//     {
//         node.children.push_back(ModelNode());
//         GenerateModelNode(child, node.children.back());
//     }
// }

}