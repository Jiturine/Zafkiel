#include "test_layer.h"

#include "editor/editor_layer.h"
#include "engine_refl_generate.h"
#include "editor_refl_generate.h"
#include "editor/editorGUI/editorGUI.h"

#include "function/input/input.h"

#include "core/application/application.h"

#include "function/render/backends/vulkan/vulkan_render_pass.h"
#include "function/render/backends/vulkan/vulkan_context.h"
#include "function/render/render_command.h"
#include "function/render/renderer.h"
#include "editor/panels/scene_panel.h"

#include "function/scene/components.h"
#include "project/project_manager.h"

#include "core/meta/serializer/binary_serializer.h"

#include "editor/function/scene/editor_scene_manager.h"

namespace Zafkiel
{
void TestLayer::OnAttach()
{
    Reflection::RegisterEditor();
    Reflection::RegisterEngine();

    WindowSpecification spec
    {
        .graphicsAPI = GraphicsAPI::Vulkan,
        .title = "Zafkiel Editor",
        .width = 1920,
        .height = 1080
    };
    window = CreateScope<EditorWindow>(spec);
    window->CreatePanels();

    Renderer::Init(GraphicsAPI::Vulkan, *window); 

    Application::KickRenderThread();
    Application::WaitRenderThreadInitFinish();
    
    Renderer::Submit([self = Ref(this)]() mutable {
        self->globalRenderResource = Renderer::GetGraphicsContext()->CreateGlobalRenderResource("assets/shaders/schema/global.zss");
        self->objectRenderResource = Renderer::GetGraphicsContext()->CreateObjectRenderResource("assets/shaders/schema/mesh_object.zss");
        self->geometryPass = CreateScope<GeometryPass>(self->globalRenderResource, self->objectRenderResource);
        self->shadingPass = CreateScope<ShadingPass>(self->globalRenderResource,
                                                     self->geometryPass->positionTexture,
                                                     self->geometryPass->normalTexture,
                                                     self->geometryPass->albedoTexture);
        self->window->GetActivePanel<ScenePanel>()->SetSceneTexture(self->shadingPass->outputColorTexture);
    });
    
    Application::KickRenderThread();
    Application::WaitRenderThread();

    ProjectManager::Init();
    const std::string editorConfigStr = FileSystem::ReadText("editor_config.yaml");
    EditorConfig config = Deserialize<EditorConfig>(editorConfigStr);

    const std::string projectConfigStr = FileSystem::ReadText(config.startProjectPath);
    ProjectConfig projConfig = Deserialize<ProjectConfig>(projectConfigStr);
    ProjectManager::CreateProject(projConfig);

    EditorAssetManager::Init();
    EditorAssetManager::SetAssetDirectory(ProjectManager::GetCurrentProject()->GetAssetDirectory());
    EditorAssetManager::RegisterAssets();
    EditorAssetManager::LoadBuiltinShader();
    // EditorAssetManager::ImportAsset("models/furina/furina.pmx");

    EditorSceneManager::Init();
    EditorSceneManager::OpenScene(projConfig.startScene);

    editorCamera = CreateScope<EditorCamera>();
    editorCamera->SetPerspective(45, 0.3f, 1000.0f);
    editorCamera->SetViewportSize(1280, 720);
    editorCamera->SetLookAtDir(vec3(0.0f, 0.0f, 1.0f));
    editorCamera->SetPosition(vec3(0.0f, 0.0f, -1.0f));

    EditorSceneManager::GetActiveScene()->GetWorld().InstantiateModel(13795713560453464113);

    Application::KickRenderThread();
    Application::WaitRenderThread();
}

void TestLayer::OnDetach()
{
    // 等待渲染线程最后一次Update中的渲染完成
    Application::KickRenderThread();
    Application::WaitRenderThread();
    Application::ClearRenderThreadSubmitQueue();
    Renderer::Submit([self = Ref(this)]() mutable {
        if (Renderer::GetGraphicsContext()->GetAPI() == GraphicsAPI::Vulkan)
        {
            Renderer::GetGraphicsContext().As<VulkanContext>().GetDevice()->GetHandle().waitIdle();
        }
        self->shadingPass = nullptr;
        self->geometryPass = nullptr;
        self->objectRenderResource = nullptr;
        self->globalRenderResource = nullptr;
    });

    EditorSceneManager::Destroy();
    EditorAssetManager::Destroy();
    ProjectManager::Destroy();
    editorCamera = nullptr;
    window->DestroyPanels();

    Renderer::Destroy();
}

FrameData TestLayer::PrepareFrameData(Observer<EditorCamera> camera)
{
    FrameData frameData;
    frameData.cameraPos = camera->GetPosition();
    frameData.viewMatrix = camera->GetViewMatrix();
    frameData.projectionMatrix = camera->GetProjectionMatrix();
    uint32_t index = 0;
    for (auto entity : SceneManager::GetActiveScene()->GetWorld().Query<TransformComponent, MeshComponent, MaterialComponent>())
    {
        Renderable renderable
        {
            .index = index,
            .entityID = entity.GetHandle(),
            .modelMatrix = entity.GetComponent<TransformComponent>().GetWorldMatrix(),
            .mesh = entity.GetComponent<MeshComponent>().mesh,
            .material = entity.GetComponent<MaterialComponent>().material,
        };
        frameData.renderables.push_back(renderable);
        index++;
    }
    return frameData;
}

void TestLayer::OnUpdate(float timestep)
{
    if(window->ShouldClose())
    {
        Application::Instance().Exit();
        return;
    }

    Application::WaitRenderThread();
    Application::KickRenderThread();

    window->PollEvents();

    editorCamera->Update(timestep);

    auto frameData = PrepareFrameData(editorCamera);

    Renderer::Submit([this, frameData]() {
        Renderer::BeginFrame();

        globalRenderResource->GetRenderResource()->SetUniform("ViewPosition", frameData.cameraPos);
        globalRenderResource->GetRenderResource()->SetUniform("ViewMatrix", frameData.viewMatrix);
        globalRenderResource->GetRenderResource()->SetUniform("ProjectionMatrix", frameData.projectionMatrix);
        globalRenderResource->GetRenderResource()->SetUniform<mat4>("ViewProjectionMatrix", frameData.projectionMatrix * frameData.viewMatrix);
        RenderCommand::BindGlobalRenderResource(globalRenderResource);

        for (auto &renderable : frameData.renderables)
        {
            objectRenderResource->SetUniform(renderable.index, "ModelMatrix", renderable.modelMatrix);
            objectRenderResource->SetUniform(renderable.index, "EntityID", (uint32_t)renderable.entityID);
        }
        objectRenderResource->UploadUniform();
        
        if (auto scenePanel = window->GetActivePanel<ScenePanel>())
        {
            geometryPass->Render(frameData);
            shadingPass->Render(frameData);
        }
        EditorGUI::BeginFrame(); 
        {
            GUIDockSpace dockspace("Hello DockSpace!");
            {
                for (auto &panel : window->panels)
                {
                    panel->Render();
                }
                GUIWindow testWindow("Test");
                EditorGUI().Button("Hello", []() {
                    Log::Info("Hello World!");
                });
            }
        }
        EditorGUI::EndFrame();
        Renderer::EndFrame();
    });

    if (auto scenePanel = window->GetActivePanel<ScenePanel>())
    {
        if (scenePanel->NeedResize())
        {
            Renderer::Submit([this, scenePanel]() mutable {
                scenePanel->UnregisterSceneTexture();
                geometryPass->Resize(scenePanel->size.x, scenePanel->size.y);
                shadingPass->Resize(scenePanel->size.x, scenePanel->size.y);
                scenePanel->SetSceneTexture(shadingPass->outputColorTexture);
            });
            editorCamera->SetViewportSize(scenePanel->size.x, scenePanel->size.y);
        }
    }
}

}
