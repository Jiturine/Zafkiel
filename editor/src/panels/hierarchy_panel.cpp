#include "hierarchy_panel.h"
#include "editorGUI/editorGUI.h"
#include "function/scene/components.h"
#include "function/engine.h"
#include "editor.h"

namespace Zafkiel
{

void HierarchyPanel::DrawEntityNode(Entity entity)
{
    Ref<Scene> scene = Engine::GetActiveScene();
    bool selected = entity == Editor::GetSelectedEntity();

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
    if (!entity.HasChildren()) flags |= ImGuiTreeNodeFlags_Leaf;
    if (selected) flags |= ImGuiTreeNodeFlags_Selected;

    std::string label = entity.HasComponent<TagComponent>() ? entity.GetComponent<TagComponent>().name : entity.GetUUID().ToString();

    GUITreeNode node((uint32_t)entity.GetHandle(), flags, label);
    node.Popup([&]() {
        EditorGUI().MenuItem("Destroy", [&]() {
            scene->GetWorld().DestroyEntity(entity);
        });
    });
    if (entity.HasChildren())
    {
        node.Expand([&]() {
            for (auto child : entity.GetChildren())
                DrawEntityNode(child);
        });
    }

    if (node.leftClicked)
    {
        Editor::SetSelectedEntity(entity);
    }
}

void HierarchyPanel::Render()
{
    GUIWindow hierarchyPanel("Hierarchy");

    Ref<Scene> scene = Engine::GetActiveScene();
    std::vector<Entity> rootEntities;
    for (auto entity : scene->GetWorld().AllEntities())
    {
        if (!entity.HasParent()) rootEntities.push_back(entity);
    }

    std::sort(rootEntities.begin(), rootEntities.end(), [](Entity a, Entity b) { return a.GetUUID() < b.GetUUID(); });

    for (auto entity : rootEntities)
    {
        DrawEntityNode(entity);
    }

    hierarchyPanel.Popup([&]() {
        EditorGUI().MenuItem("Create Entity", [&]() {
            scene->GetWorld().SpawnEntity(TransformComponent{}, TagComponent{"Empty", "Default"});
        });
    });
    hierarchyPanel.OnClickEmpty([&]() {
        Editor::SetSelectedEntity(Entity());
    });
}
}