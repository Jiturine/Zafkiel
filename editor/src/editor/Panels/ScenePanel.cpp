#include "editor/Panels/ScenePanel.h"
#include <imgui.h>
#include "ImGuizmo.h"
#include "editor/EditorGUI/EditorGUI.h"
#include "Function/Render/Renderer.h"
#include "Function/RHI/RHIResources.h"
#include "Function/Scene/Components.h"
#include "Function/Input/Input.h"

namespace Zafkiel
{

ScenePanel::ScenePanel()
{
}

void ScenePanel::UnregisterSceneTexture()
{
    GlobalRHI->UnregisterImGuiTexture(currentTexture.get());
}
void ScenePanel::SetSceneTexture(Ref<RHITexture> texture)
{
    sceneTexRef = GlobalRHI->RegisterImGuiTexture(texture.get());

    currentTexture = texture;
}

void ScenePanel::Render()
{
    GUIWindow scenePanel("Scene");

    needResize = false;
    auto newSize = scenePanel.GetContentSize();
    if (newSize != size)
    {
        needResize = true;
    }
    size = newSize;

    auto [mx, my] = ImGui::GetMousePos();
    mousePos = {mx, my};
    contentPos = scenePanel.GetContentPosition();

    hovered = scenePanel.hovered;

    EditorGUI().Image(sceneTexRef, size, vec2(0.0f, 1.0f), vec2(1.0f, 0.0f));

    // DrawGizmo(scenePanel.GetContentPosition(), size);
}

void ScenePanel::DrawGizmo(vec2 contentPosition, vec2 contextSize)
{
    // Entity selectedEntity = Editor::GetSelectedEntity();
    // if (selectedEntity && gizmoType != -1)
    // {
    //     ImGuizmo::SetOrthographic(false);
    //     ImGuizmo::SetDrawlist();

    //     ImGuizmo::SetRect(contentPosition.x, contentPosition.y, contextSize.x, contextSize.y);

    //     // Editor camera
    //     const mat4 &cameraProjection = editorCamera->GetProjectionMatrix();
    //     const mat4 &cameraView = editorCamera->GetViewMatrix();

    //     // Entity transform
    //     auto &transformComponent = selectedEntity.GetComponent<TransformComponent>();
    //     mat4 &worldMatrix = transformComponent.GetWorldMatrix();

    //     // Snapping
    //     bool snap = Input::IsKeyPressed(KeyCode::LCtrl);
    //     float snapValue = 0.5f; // Snap to 0.5m for translation/scale
    //     if (gizmoType == ImGuizmo::OPERATION::ROTATE)
    //         snapValue = 45.0f; // Snap to 45 degrees for rotation

    //     float snapValues[3] = {snapValue, snapValue, snapValue};

    //     ImGuizmo::Manipulate(cameraView.value(), cameraProjection.value(),
    //         (ImGuizmo::OPERATION)gizmoType, ImGuizmo::LOCAL, worldMatrix.value(),
    //         nullptr, snap ? snapValues : nullptr);

    //     if (ImGuizmo::IsUsing())
    //     {
    //         isUsingGizmo = true;
    //         transformComponent.SetWorldMatrix(worldMatrix);
    //     }
    //     else
    //     {
    //         isUsingGizmo = false;
    //     }
    // }
}

void ScenePanel::Update(float timestep)
{
    // if (hovered)
    // {
    //     editorCamera->Update(timestep);
    // }
    // auto spec = sceneFrameBuffer->GetSpecification();
    // if (spec.height != size.y || spec.width != size.x)
    // {
    //     sceneFrameBuffer->Resize(size.x, size.y);
    //     gBuffer->Resize(size.x, size.y);
    //     editorCamera->SetViewportSize(size.x, size.y);
    // }
    // if (hovered)
    // {
    //     if (Input::IsKeyPressed(KeyCode::Z))
    //         gizmoType = ImGuizmo::OPERATION::TRANSLATE;
    //     else if (Input::IsKeyPressed(KeyCode::X))
    //         gizmoType = ImGuizmo::OPERATION::ROTATE;
    //     else if (Input::IsKeyPressed(KeyCode::C))
    //         gizmoType = ImGuizmo::OPERATION::SCALE;
    // }
    // if (hovered && Input::IsMouseButtonDown(MouseButton::Left))
    // {
    //     float px = mousePos.x - contentPos.x;
    //     float py = sceneFrameBuffer->GetSpecification().height - (mousePos.y - contentPos.y);
    //     EntityID selectedEntity = (EntityID)sceneFrameBuffer->ReadPixel<uint32>(1, px, py);
    //     if (!isUsingGizmo && selectedEntity != entt::null)
    //     {
    //         Editor::SetSelectedEntity(Engine::GetActiveScene()->GetWorld().GetEntityByID(selectedEntity));
    //     }
    //     else if (!isUsingGizmo)
    //     {
    //         Editor::SetSelectedEntity(Entity::null);
    //     }
    // }
}
}
