#include "scene_panel.h"
#include <imgui.h>
#include "ImGuizmo.h"
#include "editorGUI/editorGUI.h"
#include "function/render/model.h"
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
    spec.attachments = {TextureFormat::RGBA8, TextureFormat::R32UI, TextureFormat::DEPTH24STENCIL8};
    spec.width = 1280;
    spec.height = 720;
    sceneFrameBuffer = Engine::GetGraphicsContext()->CreateFrameBuffer(spec);

    renderer2D = MakeRef<EditorRenderer2D>(Engine::GetGraphicsContext());
    renderer3D = MakeRef<EditorRenderer3D>();
}

void ScenePanel::Render()
{
    GUIWindow scenePanel("Scene");
    size = scenePanel.GetContentSize();
    auto [mx, my] = ImGui::GetMousePos();
    mousePos = {mx, my};
    contentPos = scenePanel.GetContentPosition();

    hovered = scenePanel.hovered;

    auto textureID = sceneFrameBuffer->GetColorAttachment(0)->GetRendererID();

    EditorGUI().Image(textureID, size, vec2(0.0f, 1.0f), vec2(1.0f, 0.0f));

    DrawGizmo(scenePanel.GetContentPosition(), size);
}

void ScenePanel::DrawGizmo(vec2 contentPosition, vec2 contextSize)
{
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
            isUsingGizmo = true;
            transformComponent.SetWorldMatrix(worldMatrix);
        }
        else
        {
            isUsingGizmo = false;
        }
    }
}

void ScenePanel::RenderScene(Ref<Scene> scene)
{
    sceneFrameBuffer->Bind();

    uint32_t nullEntity = (uint32_t)entt::null;

    Engine::GetGraphicsContext()->Clear(vec4(0.3f, 0.5f, 0.8f, 1.0f));

    sceneFrameBuffer->ClearColorAttachment(1, &nullEntity);

    glEnable(GL_DEPTH_TEST);

    renderer2D->BeginScene(editorCamera->GetProjectionMatrix() * editorCamera->GetViewMatrix());

    for (auto entity : scene->GetWorld().Query<TransformComponent, SpriteRendererComponent>())
    {
        auto &transform = entity.GetComponent<TransformComponent>();
        auto &spriterenderer2D = entity.GetComponent<SpriteRendererComponent>();
        EditorRenderer2D::QuadProps props;
        props.position = transform.GetWorldPosition();
        props.size = transform.GetWorldScale();
        props.color = spriterenderer2D.color;
        props.texture = Editor::GetProject()->GetAssetManager()->GetAsset(spriterenderer2D.texture).As<Texture2D>();
        props.entityID = (uint32_t)entity.GetHandle();
        renderer2D->DrawQuad(props);
    }
    renderer2D->EndScene();

    renderer3D->BeginScene(editorCamera->GetProjectionMatrix() * editorCamera->GetViewMatrix());
    for (auto entity : scene->GetWorld().Query<TransformComponent, MeshComponent>())
    {
        auto &transform = entity.GetComponent<TransformComponent>();
        auto &meshComp = entity.GetComponent<MeshComponent>();
        Ref<Mesh> mesh = Editor::GetProject()->GetAssetManager()->GetAsset(meshComp.mesh).As<Mesh>();

        renderer3D->DrawMesh(mesh, transform.GetWorldMatrix(), entity.GetHandle());
    }
    renderer3D->EndScene();
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
    if (hovered)
    {
        if (Input::IsKeyPressed(Scancode::Z))
            gizmoType = ImGuizmo::OPERATION::TRANSLATE;
        else if (Input::IsKeyPressed(Scancode::X))
            gizmoType = ImGuizmo::OPERATION::ROTATE;
        else if (Input::IsKeyPressed(Scancode::C))
            gizmoType = ImGuizmo::OPERATION::SCALE;
    }
    if (hovered && Input::IsMouseButtonDown(MouseButton::Left))
    {
        float px = mousePos.x - contentPos.x;
        float py = sceneFrameBuffer->GetSpecification().height - (mousePos.y - contentPos.y);
        EntityID selectedEntity = (EntityID)sceneFrameBuffer->ReadPixel<uint32_t>(1, px, py);
        if (!isUsingGizmo && selectedEntity != entt::null)
        {
            Editor::SetSelectedEntity(Engine::GetActiveScene()->GetWorld().GetEntityByID(selectedEntity));
        }
        else if (!isUsingGizmo)
        {
            Editor::SetSelectedEntity(Entity::null);
        }
    }
}

}