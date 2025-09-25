#pragma once
#include "panel.h"
#include "function/scene/scene.h"
#include "function/render/graphics_context.h"
#include "function/render/texture.h"
#include "function/render/renderer2D.h"
#include "function/render/backends/opengl/opengl_shader.h"
#include "editor_camera.h"

// debug
#include "resource/editor_asset_manager.h"

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
    Ref<Renderer2D> renderer;
    std::unique_ptr<EditorCamera> editorCamera;
    Ref<VertexArray> vertexArray;
    Ref<Shader> shader;
    Ref<Texture2D> texture;
  private:
    void DrawGizmo(vec2 contentPosition, vec2 contextSize);
    int gizmoType = -1;
    bool hovered = false;
};
}