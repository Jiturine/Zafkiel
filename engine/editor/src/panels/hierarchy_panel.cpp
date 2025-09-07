#include "hierarchy_panel.h"
#include "editorGUI/editorGUI.h"
#include "function/scene/components.h"

namespace Zafkiel
{

void HierarchyPanel::Render()
{
    GUIWindow hierarchyPanel("Hierarchy");
    for (auto entity : scene->GetWorld().AllEntities())
    {
        bool selected = entity == scene->selectedEntity;
        GUITreeNode node((uint32_t)entity.GetHandle(), ImGuiTreeNodeFlags_OpenOnArrow | (selected ? ImGuiTreeNodeFlags_Selected : 0), entity.GetComponent<TagComponent>().name);
        node.Expand([entity]() {
            EditorGUI().Text("{}", entity.GetComponent<TagComponent>().name);
        });
        if (node.clicked)
        {
            scene->selectedEntity = entity;
        }
    }
}
}