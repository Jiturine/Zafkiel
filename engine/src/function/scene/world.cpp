#include "function/scene/world.h"
#include "function/scene/components.h"
#include "core/meta/serializer/custom_serialize.h"
#include "function/render/model.h"
#include "function/script/script_engine.h"
#include "resource/asset_manager.h"
#include "resource/model_asset.h"

namespace Zafkiel
{

void World::InstantiateModelNode(const ModelAssetNode &node, Entity nodeEntity)
{
    for (auto modelMesh : node.meshes)
    {
        Entity meshEntity = SpawnEntity(TransformComponent{}, TagComponent{node.name, "Object"}, MeshComponent{modelMesh.mesh}, MaterialComponent{modelMesh.material});
        meshEntity.SetParent(nodeEntity);
    }
    for (auto child : node.children)
    {
        Entity childEntity = SpawnEntity(TransformComponent{}, TagComponent{child.name, "Object"});
        childEntity.GetComponent<TransformComponent>().SetLocalMatrix(node.localTransform);
        childEntity.SetParent(nodeEntity);
        InstantiateModelNode(child, childEntity);
    }
}

Entity World::InstantiateModel(AssetHandle model)
{
    Ref<ModelAsset> modelAsset = AssetManager::LoadAsset(model).As<ModelAsset>();
    Entity modelEntity = SpawnEntity(TransformComponent{}, TagComponent{"Model", "Object"});

    auto &rootNode = modelAsset->GetRootNode();

    InstantiateModelNode(rootNode, modelEntity);

    return modelEntity;
}

}