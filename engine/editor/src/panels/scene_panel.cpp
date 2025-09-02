#include "scene_panel.h"
#include "editorGUI/editorGUI.h"

namespace Zafkiel
{

ScenePanel::ScenePanel(Ref<GraphicsContext> context)
    : context(context)
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
    sceneFrameBuffer = context->CreateFrameBuffer(spec);

    texture = context->CreateTexture2D("assets/textures/furina.png");

    renderer = MakeRef<Renderer2D>(context);
}

void ScenePanel::Render()
{
    GUIWindow scenePanel("Scene");
    size = scenePanel.GetContentSize();

    auto textureID = sceneFrameBuffer->GetColorAttachmentRendererID();

    EditorGUI().Image(textureID, size, vec2(0.0f, 1.0f), vec2(1.0f, 0.0f));
}

void ScenePanel::RenderScene()
{
    sceneFrameBuffer->Bind();

    context->Clear(vec4(0.3f, 0.5f, 0.8f, 1.0f));

    renderer->BeginScene(editorCamera->GetProjectionMatrix() * editorCamera->GetViewMatrix());

    Renderer2D::QuadProps props;
    props.color = vec4(1.0f, 1.0f, 0.0f, 1.0f);
    props.size = vec2(1.0f, 1.0f);
    props.position = vec3(0.0f, 0.0f, 0.0f);
    props.texture = texture;
    renderer->DrawQuad(props);

    renderer->EndScene();

    sceneFrameBuffer->Unbind();
}

void ScenePanel::Update(float timestep)
{
    editorCamera->Update(timestep);
    auto spec = sceneFrameBuffer->GetSpecification();
    if (spec.height != size.y || spec.width != size.x)
    {
        sceneFrameBuffer->Resize(size.x, size.y);
        editorCamera->SetViewportSize(size.x, size.y);
    }
}

}