#include "scene_panel.h"
#include "../editorGUI/editorGUI.h"

//debug
#include "renderer/backends/opengl/opengl_shader.h"

namespace Zafkiel
{
static Ref<VertexArray> vertexArray;
static Ref<OpenGLShader> shader;
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

    // --------------------debug------------------------
    vertexArray = context->CreateVertexArray();
    uint32_t indices[] = {0, 1, 2, 2, 3, 0};
    Ref<IndexBuffer> indexBuffer = context->CreateIndexBuffer(indices, sizeof(indices) / sizeof(uint32_t));
    BufferLayout layout = {
        {ShaderDataType::Float3, "a_Position"},
        {ShaderDataType::Float4, "a_Color"}};
    float vertices[] = {
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.5f, 1.0f};
    Ref<VertexBuffer> vertexBuffer = context->CreateVertexBuffer(vertices, sizeof(vertices));
    vertexBuffer->SetLayout(layout);
    vertexArray->AddVertexBuffer(vertexBuffer);
    vertexArray->SetIndexBuffer(indexBuffer);

    shader = MakeRef<OpenGLShader>("assets/shaders/flat_shader.glsl");

    // --------------------debug------------------------
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

    context->Clear(vec4(0.1f, 0.1f, 0.1f, 1.0f));

    shader->Set("u_ViewProjection", editorCamera->GetProjectionMatrix() * editorCamera->GetViewMatrix());

    context->DrawIndexed(vertexArray, shader);

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