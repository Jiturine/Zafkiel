#include "scene_panel.h"
#include "editorGUI/editorGUI.h"
#include "function/scene/components.h"

namespace Zafkiel
{

ScenePanel::ScenePanel(Ref<GraphicsContext> context, AssetHandle handle, Ref<EditorAssetManager> editorAssetManager)
    : context(context), editorAssetManager(editorAssetManager)
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

    texture = editorAssetManager->GetAsset(handle).As<Texture2D>();

    renderer = MakeRef<Renderer2D>(context);
}

void ScenePanel::Render()
{
    GUIWindow scenePanel("Scene");
    size = scenePanel.GetContentSize();

    auto textureID = sceneFrameBuffer->GetColorAttachmentRendererID();

    EditorGUI().Image(textureID, size, vec2(0.0f, 1.0f), vec2(1.0f, 0.0f));
}

void ScenePanel::RenderScene(Ref<Scene> scene)
{
    sceneFrameBuffer->Bind();

    context->Clear(vec4(0.3f, 0.5f, 0.8f, 1.0f));

    renderer->BeginScene(editorCamera->GetProjectionMatrix() * editorCamera->GetViewMatrix());

    for (auto entity : scene->GetWorld().Query<TransformComponent, SpriteRendererComponent>())
    {
        auto &transform = entity.GetComponent<TransformComponent>();
        auto &spriteRenderer = entity.GetComponent<SpriteRendererComponent>();
        Renderer2D::QuadProps props;
        props.position = transform.position;
        props.size = transform.scale;
        props.color = spriteRenderer.color;
        props.texture = editorAssetManager->GetAsset(spriteRenderer.texture).As<Texture2D>();
        renderer->DrawQuad(props);
    }

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