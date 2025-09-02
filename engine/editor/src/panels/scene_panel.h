#pragma once
#include "panel.h"
#include "function/scene/scene.h"
#include "function/render/graphics_context.h"
#include "function/render/texture.h"
#include "function/render/renderer2D.h"
#include "function/render/backends/opengl/opengl_shader.h"
#include "editor_camera.h"

namespace Zafkiel
{
class ScenePanel : public Panel
{
  public:
    ScenePanel(Ref<GraphicsContext> context);
    void SetCurrentScene(Ref<Scene> currentScene) { scene = currentScene; }
    void Render() override;
    void RenderScene();
    void Update(float timestep);
    Ref<FrameBuffer> sceneFrameBuffer;
    Ref<GraphicsContext> context;
    Ref<Renderer2D> renderer;
    std::unique_ptr<EditorCamera> editorCamera;
    Ref<VertexArray> vertexArray;
    Ref<Shader> shader;
    Ref<Texture2D> texture;
    Ref<Scene> scene;
};
}