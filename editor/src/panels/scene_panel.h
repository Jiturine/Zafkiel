#pragma once
#include "panel.h"
#include "function/scene/scene.h"
#include "function/render/graphics_context.h"
#include "function/render/texture.h"
#include "engine_extensions/function/render/editor_renderer2D.h"
#include "engine_extensions/function/render/editor_renderer3D.h"
#include "function/render/backends/opengl/opengl_shader.h"
#include "engine_extensions/function/render/editor_camera.h"
#include <imgui.h>
#include "ImGuizmo.h"
// debug
#include "engine_extensions/resource/editor_asset_manager.h"

namespace Zafkiel
{
class ScenePanel : public Panel
{
  public:
    ScenePanel();
    void Render() override;
    void RenderScene(Ref<Scene> scene);
    void Update(float timestep);
    Ref<FrameBuffer> sceneFrameBuffer;
    Ref<EditorRenderer2D> renderer2D;
    Ref<EditorRenderer3D> renderer3D;
    std::unique_ptr<EditorCamera> editorCamera;
    Ref<VertexArray> vertexArray;
    Ref<Shader> shader;
    Ref<Texture2D> texture;
  private:
    void DrawGizmo(vec2 contentPosition, vec2 contextSize);
    int gizmoType = ImGuizmo::OPERATION::TRANSLATE;
    bool hovered = false;
    bool isUsingGizmo = false;
    vec2 mousePos;
    vec2 contentPos;
};
}