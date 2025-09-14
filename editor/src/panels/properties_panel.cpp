#include "properties_panel.h"
#include "function/scene/components.h"

namespace Zafkiel
{
void PropertiesPanel::Render()
{
    GUIWindow propertiesPanel("Properties");

    if (!scene->selectedEntity) return;
    Entity entity = scene->selectedEntity;

    DrawComponents(entity);
}

void PropertiesPanel::DrawTagComponent(TagComponent &tagComponent)
{
    EditorGUI().InputText("Name", tagComponent.name);
}

void PropertiesPanel::DrawTransformComponent(TransformComponent &transformComponent)
{
    const Type *typeInfo = GetType<TransformComponent>();
    GUITreeNode node((void *)typeInfo, ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen, typeInfo->GetName());
    node.Expand([&]() {
        EditorGUI().DragVec3("Position", transformComponent.position);

        vec3 eulerAngles = Maths::EulerDegrees(transformComponent.rotation);
        EditorGUI().DragVec3("Rotation", eulerAngles, [&]() {
            transformComponent.rotation = Maths::EulerDrgreesToQuaternion(eulerAngles);
        });

        EditorGUI().DragVec3("Scale", transformComponent.scale);
    });
}

void PropertiesPanel::DrawComponents(Entity entity)
{
    if (entity.HasComponent<TagComponent>())
    {
        DrawTagComponent(entity.GetComponent<TagComponent>());
    }
    if (entity.HasComponent<TransformComponent>())
    {
        DrawTransformComponent(entity.GetComponent<TransformComponent>());
    }
    if (entity.HasComponent<ScriptComponent>())
    {
        DrawCommonComponent<ScriptComponent>(entity.GetComponent<ScriptComponent>());
    }
    if (entity.HasComponent<SpriteRendererComponent>())
    {
        DrawCommonComponent<SpriteRendererComponent>(entity.GetComponent<SpriteRendererComponent>());
    }
}

}