#include "editor/test_layer.h"

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
#include "editor/project/project_manager.h"

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
        self->globalMaterial = Renderer::Instance().CreateGlobalMaterial("assets/shaders/schema/global.zss");
        self->objectShaderMaterial = Renderer::Instance().CreateObjectShaderMaterial("assets/shaders/schema/mesh_object.zss");
        self->geometryPass = CreateScope<GeometryPass>(self->globalMaterial, self->objectShaderMaterial);
        self->shadingPass = CreateScope<ShadingPass>(self->globalMaterial,
                                                     self->geometryPass->positionTexture,
                                                     self->geometryPass->normalTexture,
                                                     self->geometryPass->albedoTexture);
        self->shadowPass = CreateScope<ShadowPass>(self->objectShaderMaterial);
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
    // EditorAssetManager::ImportAsset("models/sponza_gltf_version/scene.gltf");

    EditorSceneManager::Init();
    EditorSceneManager::Instance().OpenScene(projConfig.startScene);

    editorCamera = CreateScope<EditorCamera>();
    editorCamera->SetPerspective(45, 0.3f, 1000.0f);
    editorCamera->SetViewportSize(1280, 720);
    editorCamera->SetLookAtDir(vec3(0.0f, 0.0f, 1.0f));
    editorCamera->SetPosition(vec3(0.0f, 1.0f, -1.0f));

    auto model = EditorSceneManager::Instance().GetActiveSceneMut()->GetWorld().InstantiateModel(15573915810613239818);
    model.GetComponent<TransformComponent>().SetScale(vec3(0.3f, 0.3f, 0.3f));
    
    EditorSceneManager::Instance().GetActiveSceneMut()->GetWorld().SpawnEntity(TransformComponent(vec3(0.0f)), LightComponent{LightType::Directional, vec3(1.0f), 1.0f, vec3(0.5f, -0.5f, 0.5f)});

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
        self->shadowPass = nullptr;
        self->shadingPass = nullptr;
        self->geometryPass = nullptr;
    });

    EditorSceneManager::Destroy();
    EditorAssetManager::Destroy();
    ProjectManager::Destroy();
    editorCamera = nullptr;
    window->DestroyPanels();

    Renderer::Destroy();
}

FrameData TestLayer::PrepareFrameData(Borrow<EditorCamera> camera)
{
    FrameData frameData;
    frameData.cameraPos = camera->GetPosition();
    frameData.viewMatrix = camera->GetViewMatrix();
    frameData.projectionMatrix = camera->GetProjectionMatrix();
    uint32_t index = 0;
    for (auto entity : SceneManager::Instance().GetActiveScene()->GetWorld().Query<TransformComponent, MeshComponent, MaterialComponent>())
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
    for (auto entity : SceneManager::Instance().GetActiveScene()->GetWorld().Query<LightComponent>())
    {
        auto lightComponent = entity.GetComponent<LightComponent>();
        if (lightComponent.type == LightType::Directional)
        {
            frameData.directionalLight = {lightComponent.direction, lightComponent.color, lightComponent.intensity};
            break;
        }
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

    auto frameData = PrepareFrameData(Borrow(editorCamera));

    Renderer::Submit([this, frameData]() {
        Renderer::Instance().BeginFrame();

        mat4 viewProjMat = frameData.projectionMatrix * frameData.viewMatrix;
        Renderer::Instance().SetUniformFromGlobalMaterial(globalMaterial, "ViewPosition", ShaderFundamentalType::Float3, frameData.cameraPos);
        Renderer::Instance().SetUniformFromGlobalMaterial(globalMaterial, "ViewMatrix", ShaderFundamentalType::Mat4, frameData.viewMatrix);
        Renderer::Instance().SetUniformFromGlobalMaterial(globalMaterial, "ProjectionMatrix", ShaderFundamentalType::Mat4, frameData.projectionMatrix);
        Renderer::Instance().SetUniformFromGlobalMaterial(globalMaterial, "ViewProjectionMatrix", ShaderFundamentalType::Mat4, viewProjMat);
        Renderer::Instance().CmdBindGlobalMaterial(globalMaterial);

        for (auto &renderable : frameData.renderables)
        {
            Renderer::Instance().SetUniformFromObjectShaderMaterial(objectShaderMaterial, renderable.index, "ModelMatrix", ShaderFundamentalType::Mat4, renderable.modelMatrix);
            Renderer::Instance().SetUniformFromObjectShaderMaterial(objectShaderMaterial, renderable.index, "EntityID", ShaderFundamentalType::UInt, (uint32_t)renderable.entityID);
        }
        Renderer::Instance().UploadObjectShaderMaterialUniform(objectShaderMaterial);
        
        if (auto scenePanel = window->GetActivePanel<ScenePanel>())
        {
            geometryPass->Render(frameData);
            shadowPass->Render(frameData);
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
        Renderer::Instance().EndFrame();
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
