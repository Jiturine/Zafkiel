#include "Function/Scene/World.h"
#include "Function/Scene/Components.h"
#include "Core/Meta/Serializer/CustomSerialize.h"
#include "Function/Render/Model.h"
#include "Function/Script/ScriptEngine.h"
#include "Resource/AssetManager.h"
#include "Resource/ModelAsset.h"

namespace Zafkiel
{

void World::InstantiateModelNode(const ModelAssetNode &node, Entity nodeEntity)
{
    nodeEntity.GetComponent<TransformComponent>().SetLocalMatrix(node.localTransform);

    for (auto modelMesh : node.meshes)
    {
        Entity meshEntity = SpawnEntity(TransformComponent{}, TagComponent{node.name, "Object"}, MeshComponent{modelMesh.mesh}, MaterialComponent{modelMesh.material});
        meshEntity.SetParent(nodeEntity);
    }
    for (auto child : node.children)
    {
        Entity childEntity = SpawnEntity(TransformComponent{}, TagComponent{child.name, "Object"});
        childEntity.SetParent(nodeEntity);
        InstantiateModelNode(child, childEntity);
    }
}

Entity World::InstantiateModel(AssetHandle model)
{
    Ref<ModelAsset> modelAsset = AssetManager::Instance().LoadAsset(model).As<ModelAsset>();
    Entity modelEntity = SpawnEntity(TransformComponent{}, TagComponent{"Model", "Object"});

    auto &rootNode = modelAsset->GetRootNode();

    InstantiateModelNode(rootNode, modelEntity);

    return modelEntity;
}

}