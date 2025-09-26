#include "scene_panel.h"
#include <imgui.h>
#include "ImGuizmo.h"
#include "editorGUI/editorGUI.h"
#include "function/scene/components.h"
#include "function/input/input.h"
#include "editor.h"

namespace Zafkiel
{

ScenePanel::ScenePanel()
{
    editorCamera = std::make_unique<EditorCamera>();
    editorCamera->SetPerspective(45, 0.001f, 1000.0f);
    editorCamera->SetViewportSize(1280, 720);
    editorCamera->SetLookAtDir(vec3(0.0f, 0.0f, 1.0f));
    editorCamera->SetPosition(vec3(0.0f, 0.0f, -1.0f));

    FrameBufferSpecification spec;
    spec.attachments = {FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::Depth};
    spec.width = 1280;
    spec.height = 720;
    sceneFrameBuffer = Engine::GetGraphicsContext()->CreateFrameBuffer(spec);

    renderer = MakeRef<Renderer2D>(Engine::GetGraphicsContext());
}

void ScenePanel::Render()
{
    GUIWindow scenePanel("Scene");
    size = scenePanel.GetContentSize();
    hovered = scenePanel.hovered;

    auto textureID = sceneFrameBuffer->GetColorAttachmentRendererID();

    EditorGUI().Image(textureID, size, vec2(0.0f, 1.0f), vec2(1.0f, 0.0f));

    DrawGizmo(scenePanel.GetContentPosition(), size);
}

void ScenePanel::DrawGizmo(vec2 contentPosition, vec2 contextSize)
{
    gizmoType = ImGuizmo::OPERATION::TRANSLATE;
    Entity selectedEntity = Editor::GetSelectedEntity();
    if (selectedEntity && gizmoType != -1)
    {
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();

        ImGuizmo::SetRect(contentPosition.x, contentPosition.y, contextSize.x, contextSize.y);

        // Editor camera
        const mat4 &cameraProjection = editorCamera->GetProjectionMatrix();
        const mat4 &cameraView = editorCamera->GetViewMatrix();

        // Entity transform
        auto &transformComponent = selectedEntity.GetComponent<TransformComponent>();
        mat4 &worldMatrix = transformComponent.GetWorldMatrix();

        // Snapping
        bool snap = Input::IsKeyPressed(Scancode::LCTRL);
        float snapValue = 0.5f; // Snap to 0.5m for translation/scale
        if (gizmoType == ImGuizmo::OPERATION::ROTATE)
            snapValue = 45.0f; // Snap to 45 degrees for rotation

        float snapValues[3] = {snapValue, snapValue, snapValue};

        ImGuizmo::Manipulate(cameraView.value(), cameraProjection.value(),
            (ImGuizmo::OPERATION)gizmoType, ImGuizmo::LOCAL, worldMatrix.value(),
            nullptr, snap ? snapValues : nullptr);

        if (ImGuizmo::IsUsing())
        {
            transformComponent.SetWorldMatrix(worldMatrix);
        }
    }
}

void ScenePanel::RenderScene(Ref<Scene> scene)
{
    sceneFrameBuffer->Bind();

    Engine::GetGraphicsContext()->Clear(vec4(0.3f, 0.5f, 0.8f, 1.0f));

    renderer->BeginScene(editorCamera->GetProjectionMatrix() * editorCamera->GetViewMatrix());

    for (auto entity : scene->GetWorld().Query<TransformComponent, SpriteRendererComponent>())
    {
        auto &transform = entity.GetComponent<TransformComponent>();
        auto &spriteRenderer = entity.GetComponent<SpriteRendererComponent>();
        Renderer2D::QuadProps props;
        props.position = transform.GetWorldPosition();
        props.size = transform.GetWorldScale();
        props.color = spriteRenderer.color;
        props.texture = Editor::GetProject()->GetAssetManager()->GetAsset(spriteRenderer.texture).As<Texture2D>();
        renderer->DrawQuad(props);
    }

    renderer->EndScene();

    sceneFrameBuffer->Unbind();
}

void ScenePanel::Update(float timestep)
{
    if (hovered)
    {
        editorCamera->Update(timestep);
    }
    auto spec = sceneFrameBuffer->GetSpecification();
    if (spec.height != size.y || spec.width != size.x)
    {
        sceneFrameBuffer->Resize(size.x, size.y);
        editorCamera->SetViewportSize(size.x, size.y);
    }
}

}