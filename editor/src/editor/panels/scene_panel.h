#pragma once
#include "function/render/surface_material.h"
#include "editor/panels/panel.h"
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
    void SetSceneTexture(RenderHandle texture);
    void RenderScene(Ref<Scene> scene);
    void Update(float timestep);
    bool NeedResize() const { return needResize; }

    std::vector<ImTextureRef> sceneTexRefs;
    RenderHandle currentTexture;
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
