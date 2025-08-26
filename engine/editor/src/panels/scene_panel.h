#pragma once
#include "panel.h"
#include "renderer/graphics_context.h"
#include "editor_camera.h"

namespace Zafkiel
{
class ScenePanel : public Panel
{
  public:
    ScenePanel(Ref<GraphicsContext> context);
    void Render() override;
    void RenderScene();
    void Update(float timestep);
    Ref<FrameBuffer> sceneFrameBuffer;
    Ref<GraphicsContext> context;
    std::unique_ptr<EditorCamera> editorCamera;
};
}