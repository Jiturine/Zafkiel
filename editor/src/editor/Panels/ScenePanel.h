#pragma once
#include "editor/Panels/Panel.h"
#include "Function/Scene/Scene.h"
#include "Function/RHI/RHIResources.h"
#include "Function/RHI/Backends/OpenGL/OpenGLShader.h"
#include "editor/Function/Render/EditorCamera.h"
#include <imgui.h>
#include <ImGuizmo.h>
// debug
#include "editor/Resource/EditorAssetManager.h"

namespace Zafkiel
{
class ScenePanel : public Panel
{
  public:
    ScenePanel();
    void Render() override;

    void UnregisterSceneTexture();
    void SetSceneTexture(Ref<RHITexture> texture);

    void Update(float timestep);
    
    bool NeedResize() const { return needResize; }

    ImTextureRef sceneTexRef;
  
    Ref<RHITexture> currentTexture;

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
