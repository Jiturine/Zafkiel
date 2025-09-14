#include "hierarchy_panel.h"
#include "editorGUI/editorGUI.h"
#include "function/scene/components.h"

namespace Zafkiel
{

void HierarchyPanel::DrawEntityNode(Entity entity)
{
    bool selected = entity == scene->selectedEntity;

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
    if (!entity.HasChildren()) flags |= ImGuiTreeNodeFlags_Leaf;
    if (selected) flags |= ImGuiTreeNodeFlags_Selected;

    GUITreeNode node((uint32_t)entity.GetHandle(), flags, entity.GetComponent<TagComponent>().name);
    if (entity.HasChildren())
    {
        node.Expand([&]() {
            for (auto child : entity.GetChildren())
                DrawEntityNode(child);
        });
    }
    node.Popup([&]() {
        EditorGUI().MenuItem("Destroy", [&]() {
            scene->GetWorld().DestroyEntity(entity);
        });
    });
    if (node.leftClicked)
    {
        scene->selectedEntity = entity;
    }
}

void HierarchyPanel::Render()
{
    GUIWindow hierarchyPanel("Hierarchy");
    for (auto entity : scene->GetWorld().AllEntities())
    {
        if (!entity.HasParent())
            DrawEntityNode(entity);
    }

    hierarchyPanel.Popup([&]() {
        EditorGUI().MenuItem("Create Entity", [&]() {
            scene->GetWorld().SpawnEntity(TransformComponent{}, TagComponent{"Empty", "Default"});
        });
    });
    hierarchyPanel.OnClickEmpty([&]() {
        scene->selectedEntity = Entity();
    });
}
}