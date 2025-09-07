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

void PropertiesPanel::DrawComponents(Entity entity)
{
    if (entity.HasComponent<TagComponent>())
    {
        DrawTagComponent(entity.GetComponent<TagComponent>());
    }
    if (entity.HasComponent<TransformComponent>())
    {
        DrawCommonComponent<TransformComponent>(entity.GetComponent<TransformComponent>());
    }
    if (entity.HasComponent<UUIDComponent>())
    {
        DrawCommonComponent<UUIDComponent>(entity.GetComponent<UUIDComponent>());
    }
    if (entity.HasComponent<ScriptComponent>())
    {
        DrawCommonComponent<ScriptComponent>(entity.GetComponent<ScriptComponent>());
    }
}

}