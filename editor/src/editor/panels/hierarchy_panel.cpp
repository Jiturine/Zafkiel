#include "hierarchy_panel.h"
#include "editor/editorGUI/editorGUI.h"
#include "function/scene/components.h"
#include "editor/editor_context/editor_context.h"
#include "function/scene/scene_manager.h"

namespace Zafkiel
{

static bool ContainsEntityRecursive(Entity parent, Entity target)
{
    if (parent == target)
        return true;

    for (auto child : parent.GetChildren())
    {
        if (ContainsEntityRecursive(child, target))
            return true;
    }
    return false;
}

void HierarchyPanel::DrawEntityNode(Entity entity)
{
    auto scene = SceneManager::GetActiveScene();
    auto &selectionContext = EditorContext::GetSelectionContext();

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
    if (!entity.HasChildren()) flags |= ImGuiTreeNodeFlags_Leaf;
    if (selectionContext.type == SelectionContext::Type::Entity && selectionContext.entity == entity) flags |= ImGuiTreeNodeFlags_Selected;
    if (selectionContext.type == SelectionContext::Type::Entity) 
    {
        bool expand = ContainsEntityRecursive(entity, selectionContext.entity);
        if (expand) flags |= ImGuiTreeNodeFlags_DefaultOpen;
    }

    std::string label = entity.HasComponent<TagComponent>() ? entity.GetComponent<TagComponent>().name : entity.GetUUID().ToString();

    GUITreeNode node((uint32_t)entity.GetHandle(), flags, label);
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
    {
        ImGui::SetDragDropPayload("Entity", &entity, sizeof(entity));
        ImGui::EndDragDropSource();
    }
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("Entity"))
        {
            Entity otherEntity = *(Entity *)payload->Data;
            if (!ContainsEntityRecursive(otherEntity, entity))
                otherEntity.SetParent(entity);
        }
        ImGui::EndDragDropTarget();
    }

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
        EditorContext::SetSelectedEntity(entity);
    }
}

void HierarchyPanel::Render()
{
    GUIWindow hierarchyPanel("Hierarchy");

    auto scene = SceneManager::GetActiveScene();
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

    hierarchyPanel.OnClickEmpty([&]() {
        EditorContext::SetSelectedEntity(Entity());
    });

    EditorGUI().InvisibleButton("empty drop", hierarchyPanel.GetContentSize());
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("Entity"))
        {
            Entity entity = *(Entity *)payload->Data;
            entity.SetParent(Entity::null);
        }
        ImGui::EndDragDropTarget();
    }
    if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
    {
        ImGui::OpenPopup("Click Empty Popup");
    }
    if (ImGui::BeginPopup("Click Empty Popup"))
    {
        if (ImGui::MenuItem("Create Entity"))
        {
            scene->GetWorld().SpawnEntity(TransformComponent{}, TagComponent{"Empty", "Default"});
        }
        ImGui::EndPopup();
    }
}
}