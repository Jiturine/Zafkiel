#include "editor/panels/scene_panel.h"
#include <imgui.h>
#include "ImGuizmo.h"
#include "editor/editorGUI/editorGUI.h"
#include "function/render/frame_buffer.h"
#include "function/render/image.h"
#include "function/render/surface_material.h"
#include "function/render/model.h"
#include "function/render/renderer.h"
#include "function/render/texture.h"
#include "function/scene/components.h"
#include "function/input/input.h"

namespace Zafkiel
{

ScenePanel::ScenePanel()
{
}

void ScenePanel::UnregisterSceneTexture()
{
    Renderer::Instance().UnregisterImGuiTexture(currentTexture);
}
void ScenePanel::SetSceneTexture(RenderHandle texture)
{
    sceneTexRefs = Renderer::Instance().RegisterImGuiTexture(texture);
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

    EditorGUI().Image(sceneTexRefs[0], size, vec2(0.0f, 1.0f), vec2(1.0f, 0.0f));

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

void ScenePanel::RenderScene(Ref<Scene> scene)
{
    // glEnable(GL_DEPTH_TEST);

    // // Deferred Shading
    // gBuffer->Bind();

    // Engine::GetGraphicsContext()->Clear();

    // uint32_t nullEntity = (uint32_t)entt::null;
    // gBuffer->ClearColorAttachment(1, &nullEntity);

    // for (auto entity : scene->GetWorld().Query<TransformComponent, MeshComponent, MaterialComponent>())
    // {
    //     auto &transform = entity.GetComponent<TransformComponent>();
    //     auto &meshComp = entity.GetComponent<MeshComponent>();
    //     auto &materialComp = entity.GetComponent<MaterialComponent>();
    //     Ref<Mesh> mesh = Editor::GetProject()->GetAssetManager()->GetAsset(meshComp.mesh).As<Mesh>();
    //     Ref<Material> material = Editor::GetProject()->GetAssetManager()->GetAsset(materialComp.material).As<Material>();

    //     gBufferShader->Bind();
    //     gBufferShader->Set("u_ViewProjection", editorCamera->GetProjectionMatrix() * editorCamera->GetViewMatrix());
    //     gBufferShader->Set("u_Model", transform.GetWorldMatrix());
    //     gBufferShader->Set("u_EntityID", (uint32_t)entity.GetHandle());
    //     gBufferShader->Set("u_DiffuseTex", 0);

    //     if (material->HasTexture("diffuseTexture"))
    //     {
    //         Scope<Texture2D> diffuseTexture = Engine::GetAssetManager()->GetAsset(material->GetTexture("diffuseTexture")).As<Texture2D>();
    //         uint32_t texID = diffuseTexture->GetRendererID();
    //         diffuseTexture->Bind(0);
    //     }
    //     else
    //     {
    //         renderer3D->whiteTexture->Bind(0);
    //     }

    //     Ref<VertexArray> vertexArray = mesh->GetVertexArray();
    //     Engine::GetGraphicsContext()->DrawIndexed(vertexArray, gBufferShader);
    // }

    // sceneFrameBuffer->Bind();
    // Engine::GetGraphicsContext()->Clear();
    // sceneFrameBuffer->ClearColorAttachment(1, &nullEntity);

    // LightComponent *mainDirLight = nullptr;
    // for (auto entity : scene->GetWorld().Query<LightComponent>())
    // {
    //     if (auto &lightComponent = entity.GetComponent<LightComponent>(); lightComponent.type == LightType::Directional)
    //     {
    //         mainDirLight = &lightComponent;
    //         break;
    //     }
    // }
    // if (mainDirLight)
    // {
    //     deferredShader->Bind();
    //     deferredShader->Set("u_DirLight.color", mainDirLight->color);
    //     deferredShader->Set("u_DirLight.intensity", mainDirLight->intensity);
    //     deferredShader->Set("u_DirLight.direction", mainDirLight->direction);
    // }
    // else
    // {
    //     deferredShader->Bind();
    //     deferredShader->Set("u_DirLight.color", vec3(1.0f));
    //     deferredShader->Set("u_DirLight.intensity", 1.0f);
    //     deferredShader->Set("u_DirLight.direction", vec3(0.0f, 0.0f, 1.0f));
    // }

    // deferredShader->Bind();
    // gBuffer->GetColorAttachment(0)->Bind(0);
    // gBuffer->GetColorAttachment(1)->Bind(1);
    // gBuffer->GetColorAttachment(2)->Bind(2);
    // gBuffer->GetColorAttachment(3)->Bind(3);
    // deferredShader->Set("gPosition", 0);
    // deferredShader->Set("gNormal", 1);
    // deferredShader->Set("gAlbedo", 2);
    // deferredShader->Set("gEntityID", 3);
    // deferredShader->Set("u_ViewPos", editorCamera->GetPosition());

    // Engine::GetGraphicsContext()->DrawIndexed(screenQuadVertexArray, deferredShader);

    // sceneFrameBuffer->Unbind();

    // --------------------------------

    // renderer2D->BeginScene(editorCamera->GetProjectionMatrix() * editorCamera->GetViewMatrix());

    // for (auto entity : scene->GetWorld().Query<TransformComponent, SpriteRendererComponent>())
    // {
    //     auto &transform = entity.GetComponent<TransformComponent>();
    //     auto &spriterenderer2D = entity.GetComponent<SpriteRendererComponent>();
    //     EditorRenderer2D::QuadProps props;
    //     props.position = transform.GetWorldPosition();
    //     props.size = transform.GetWorldScale();
    //     props.color = spriterenderer2D.color;
    //     props.texture = Editor::GetProject()->GetAssetManager()->GetAsset(spriterenderer2D.texture).As<Texture2D>();
    //     props.entityID = (uint32_t)entity.GetHandle();
    //     renderer2D->DrawQuad(props);
    // }
    // renderer2D->EndScene();

    // renderer3D->BeginScene(editorCamera->GetProjectionMatrix() * editorCamera->GetViewMatrix());
    // auto entities = scene->GetWorld().Query<LightComponent>();

    // LightComponent *mainDirLight = nullptr;
    // for (auto entity : entities)
    // {
    //     if (auto &lightComponent = entity.GetComponent<LightComponent>(); lightComponent.type == LightType::Directional)
    //     {
    //         mainDirLight = &lightComponent;
    //         break;
    //     }
    // }

    // glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    // glStencilFunc(GL_ALWAYS, 1, 0xFF); // 所有的片段都应该更新模板缓冲
    // glStencilMask(0xFF);               // 启用模板缓冲写入

    // for (auto entity : scene->GetWorld().Query<TransformComponent, MeshComponent, MaterialComponent>())
    // {
    //     auto &transform = entity.GetComponent<TransformComponent>();
    //     auto &meshComp = entity.GetComponent<MeshComponent>();
    //     auto &materialComp = entity.GetComponent<MaterialComponent>();
    //     Ref<Mesh> mesh = Editor::GetProject()->GetAssetManager()->GetAsset(meshComp.mesh).As<Mesh>();
    //     Ref<Material> material = Editor::GetProject()->GetAssetManager()->GetAsset(materialComp.material).As<Material>();

    //     if (mainDirLight)
    //     {
    //         material->GetShader()->Bind();
    //         material->GetShader()->Set("u_DirLight.color", mainDirLight->color);
    //         material->GetShader()->Set("u_DirLight.intensity", mainDirLight->intensity);
    //         material->GetShader()->Set("u_DirLight.direction", mainDirLight->direction);
    //     }
    //     else
    //     {
    //         material->GetShader()->Bind();
    //         material->GetShader()->Set("u_DirLight.color", vec3(1.0f));
    //         material->GetShader()->Set("u_DirLight.intensity", 1.0f);
    //         material->GetShader()->Set("u_DirLight.direction", vec3(0.0f, 0.0f, 1.0f));
    //     }

    //     renderer3D->DrawMesh(mesh, material, transform.GetWorldMatrix(), editorCamera->GetPosition(), entity.GetHandle());

    //     // normal display
    //     normalDisplayShader->Bind();
    //     normalDisplayShader->Set("u_Model", transform.GetWorldMatrix());
    //     normalDisplayShader->Set("u_View", editorCamera->GetViewMatrix());
    //     normalDisplayShader->Set("u_Projection", editorCamera->GetProjectionMatrix());

    //     Engine::GetGraphicsContext()->DrawIndexed(mesh->GetVertexArray(), normalDisplayShader);
    // }

    // glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    // glStencilMask(0x00); // 禁止模板缓冲的写入

    // for (auto entity : scene->GetWorld().Query<TransformComponent, MeshComponent>())
    // {
    //     auto &transform = entity.GetComponent<TransformComponent>();
    //     auto &meshComp = entity.GetComponent<MeshComponent>();
    //     Ref<Mesh> mesh = Editor::GetProject()->GetAssetManager()->GetAsset(meshComp.mesh).As<Mesh>();

    //     renderer3D->DrawOutline(mesh, transform.GetWorldMatrix());
    // }

    // glStencilMask(0xFF);

    // renderer3D->EndScene();

    // sceneFrameBuffer->Unbind();
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
    //     EntityID selectedEntity = (EntityID)sceneFrameBuffer->ReadPixel<uint32_t>(1, px, py);
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
