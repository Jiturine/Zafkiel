#pragma once
#include "function/render/material.h"
#include "panel.h"
#include "function/scene/scene.h"
#include "function/render/graphics_context.h"
#include "function/render/texture.h"
#include "editor/function/render/editor_renderer2D.h"
#include "editor/function/render/editor_renderer3D.h"
#include "function/render/backends/opengl/opengl_shader.h"
#include "editor/function/render/editor_camera.h"
#include <imgui.h>
#include "ImGuizmo.h"
// debug
#include "editor/resource/editor_asset_manager.h"

namespace Zafkiel
{
class ScenePanel : public Panel
{
  public:
    ScenePanel();
    void Render() override;

    void UnregisterSceneTexture();
    void SetSceneTexture(const Scope<Texture2D> &texture);
    void RenderScene(Ref<Scene> scene);
    void Update(float timestep);
    bool NeedResize() const { return needResize; }
    // Ref<FrameBuffer> gBuffer;
    // std::unique_ptr<EditorCamera> editorCamera;
    // Ref<Shader> invertColorShader;
    // Ref<Shader> skyBoxShader;
    // Ref<Shader> normalDisplayShader;
    // Ref<Shader> gBufferShader;
    // Ref<Shader> deferredShader;
    // Ref<CubeMap> skyBox;

    std::vector<ImTextureRef> sceneTexRefs;
    Observer<Texture2D> currentTexture;
  private:
    void DrawGizmo(vec2 contentPosition, vec2 contextSize);
    int gizmoType = ImGuizmo::OPERATION::TRANSLATE;
    bool hovered = false;
    bool isUsingGizmo = false;
    vec2 mousePos;
    vec2 contentPos;
    bool needResize = false;
};
}
