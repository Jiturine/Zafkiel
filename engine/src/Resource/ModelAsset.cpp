#include "Resource/ModelAsset.h"
#include "Core/Meta/Serializer/YamlSerializer.h"

namespace Zafkiel 
{

ModelAsset::ModelAsset(AssetHandle handle, const std::string &data) 
    : Asset(handle)
{
    YamlDeserializer modelData(data);

    auto &rootNodeData = modelData["Root"];

    LoadModelNode(rootNode, rootNodeData);
}

ModelAsset::ModelAsset(AssetHandle handle, ModelAssetNode node)
    : Asset(handle), rootNode(node)
{
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


}