#if 0
#include "editor/TestLayer.h"

#include "editor/EditorLayer.h"
#include "EngineReflGenerate.h"
#include "EditorReflGenerate.h"
#include "editor/EditorGUI/EditorGUI.h"

#include "Platform/Input/Input.h"

#include "Core/Application/Application.h"

#include "Function/RHI/Backends/Vulkan/VulkanRenderPass.h"
#include "Function/Render/Renderer.h"

#include "Function/Scene/Components.h"
#include "editor/Project/ProjectManager.h"

#include "Core/Meta/Serializer/BinarySerializer.h"

#include "editor/Function/Scene/EditorSceneManager.h"

namespace Zafkiel
{
void TestLayer::OnAttach()
{
    Reflection::RegisterEditor();
    Reflection::RegisterEngine();

    PlatformWindowSpecification spec
    {
        .graphicsAPI = GraphicsAPI::Vulkan,
        .title = "Zafkiel Editor",
        .width = 1920,
        .height = 1080
    };
    window = CreateScope<EditorWindow>(spec);
    window->CreatePanels();

    Renderer::Init(GraphicsAPI::Vulkan); 

    Application::Instance().KickRenderThread();
    Application::Instance().WaitRenderThreadInitFinish();
    
    Renderer::Submit([self = Ref(this)]() mutable {
        self->geometryPass = CreateScope<GeometryPass>();
        self->shadowPass = CreateScope<ShadowPass>();
        self->shadingPass = CreateScope<ShadingPass>(self->geometryPass.get(), self->shadowPass.get());
        self->postProcessingPass = CreateScope<PostProcessingPass>(self->shadingPass.get());
        self->uiPass = CreateScope<UIPass>();
        // self->window->GetActivePanel<ScenePanel>()->SetSceneTexture(self->uiPass->outputTexture);

        auto globalUniformBlock = static_cast<ShaderReflection::UniformBlock *>(self->geometryPass->pbrPipeline->GetShaders()[ShaderStage::Vertex]->GetResourceTable().GetResourceType("GlobalUBO")); // TODO: 单独的保存ResourceType类型，可能有问题

        self->globalUniformBufferContent = CreateRef<UniformBufferContent>(globalUniformBlock);
        RHIBufferDesc globalUBODesc 
        {
            .size = globalUniformBlock->GetSize(),
            .usages = BufferUsageFlags::UniformBuffer | BufferUsageFlags::CPUAccessible | BufferUsageFlags::Dynamic,
        };
        self->globalUniformBuffer = GlobalRHICmdList->CreateBuffer(globalUBODesc);

        auto objectUniformBlock = static_cast<ShaderReflection::UniformBlock *>(self->geometryPass->pbrPipeline->GetShaders()[ShaderStage::Vertex]->GetResourceTable().GetResourceType("MeshObjectUBO"));

        self->objectUniformBufferContent = GlobalRHI->CreateDynamicUniformBufferContent(maxObjectSize, objectUniformBlock);
        RHIBufferDesc objectUBODesc 
        {
            .size = self->objectUniformBufferContent->GetData().Size<uint8>(),
            .usages = BufferUsageFlags::UniformBuffer | BufferUsageFlags::CPUAccessible | BufferUsageFlags::Dynamic,
        };
        self->objectUniformBuffer = GlobalRHICmdList->CreateBuffer(objectUBODesc);
    });
    
    Application::Instance().KickRenderThread();
    Application::Instance().WaitRenderThread();

    ProjectManager::Init();
    const std::string editorConfigStr = FileSystem::ReadText("editor_config.yaml");
    EditorConfig config = Deserialize<EditorConfig>(editorConfigStr);

    const std::string projectConfigStr = FileSystem::ReadText(config.startProjectPath);
    ProjectConfig projConfig = Deserialize<ProjectConfig>(projectConfigStr);
    ProjectManager::CreateProject(projConfig);

    EditorAssetManager::Init();
    EditorAssetManager::Instance().SetAssetDirectory(ProjectManager::GetCurrentProject()->GetAssetDirectory());
    EditorAssetManager::Instance().RegisterAssets();
    // EditorAssetManager::Instance().LoadBuiltinShader();
    // EditorAssetManager::Instance().ImportModel("models/main_sponza_bin/test.glb", ShaderFamily::PBR);
    // EditorAssetManager::Instance().ImportModel("models/main_sponza/NewSponza_Main_glTF_003.gltf", ShaderFamily::PBR);
    // EditorAssetManager::Instance().ImportModel("models/lantern/Lantern.gltf", ShaderFamily::PBR);

    EditorSceneManager::Init();
    EditorSceneManager::Instance().OpenScene(projConfig.startScene);

    editorCamera = CreateScope<EditorCamera>();
    editorCamera->SetPerspective(45, 0.3f, 1000.0f);
    editorCamera->SetViewportSize(1280, 720);
    editorCamera->SetLookAtDir(vec3(0.0f, 0.0f, -1.0f));
    editorCamera->SetPosition(vec3(5.0f, 10.0f, 15.0f)); // 移动相机到能看到模型的位置

    auto model = EditorSceneManager::Instance().GetActiveScene().GetWorld().InstantiateModel(4173704841822167084);
    model.GetComponent<TransformComponent>().SetScale(vec3(0.5f, 0.5f, 0.5f));
    
    EditorSceneManager::Instance().GetActiveScene().GetWorld().SpawnEntity(TransformComponent(vec3(0.0f)), LightComponent{LightType::Directional, vec3(1.0f), 1.0f, vec3(0.5f, -0.5f, 0.5f)});

    Application::Instance().KickRenderThread();
    Application::Instance().WaitRenderThread();
}

void TestLayer::OnDetach()
{
    // 等待渲染线程最后一次Update中的渲染完成
    Application::Instance().KickRenderThread();
    Application::Instance().WaitRenderThread();
    Application::Instance().ClearRenderThreadSubmitQueue();
    Renderer::Submit([self = Ref(this)]() mutable {
        GlobalRHICmdList->SubmitAndWaitIdle();
        self->uiPass = nullptr;
        self->postProcessingPass = nullptr;
        self->shadowPass = nullptr;
        self->shadingPass = nullptr;
        self->geometryPass = nullptr;

        self->globalUniformBufferContent = nullptr;
        self->globalUniformBuffer = nullptr;
        self->objectUniformBufferContent = nullptr;
        self->objectUniformBuffer = nullptr;
    });

    EditorSceneManager::Destroy();
    EditorAssetManager::Destroy();
    ProjectManager::Destroy();
    editorCamera = nullptr;
    window->DestroyPanels();

    Renderer::Destroy();
}

FrameData TestLayer::PrepareFrameData(EditorCamera &camera)
{
    FrameData frameData;
    frameData.cameraPos = camera.GetPosition();
    frameData.viewMatrix = camera.GetViewMatrix();
    frameData.projectionMatrix = camera.GetProjectionMatrix();
    uint32 index = 0;
    for (auto entity : SceneManager::Instance().GetActiveScene().GetWorld().Query<TransformComponent, MeshComponent, MaterialComponent>())
    {
        Renderable renderable
        {
            .index = index,
            .entityID = entity.GetHandle(),
            .modelMatrix = entity.GetComponent<TransformComponent>().GetWorldMatrix(),
            .meshAssetHandle = entity.GetComponent<MeshComponent>().mesh,
            .materialAssetHandle = entity.GetComponent<MaterialComponent>().material,
        };
        frameData.renderables.push_back(renderable);
        index++;
    }
    for (auto entity : SceneManager::Instance().GetActiveScene().GetWorld().Query<LightComponent>())
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
        Application::Instance().Instance().Exit();
        return;
    }

    Application::Instance().WaitRenderThread();
    Application::Instance().KickRenderThread();

    window->PollEvents();

    editorCamera->Update(timestep);

    auto frameData = PrepareFrameData(*editorCamera.get());

    // 第一个提交批次：预创建所有resources
    Renderer::Submit([this, frameData]() {
        MaterialDesc pbrMaterialDesc
        {
            .textureMap
            {
                {"DiffuseTexture", "uDiffuseTexture"},
                {"NormalTexture", "uNormalTexture"},
                {"MetalnessTexture", "uMetalnessTexture"},
                {"RoughnessTexture", "uRoughnessTexture"},
            }
        };
        for (auto &renderable : frameData.renderables)
        {
            Renderer::Instance().GetOrCreateMesh(renderable.meshAssetHandle);
            Renderer::Instance().GetOrCreateMaterial(renderable.materialAssetHandle, pbrMaterialDesc);
        }
        GlobalRHICmdList->Submit();
    });

    // 第二个提交批次：实际的渲染
    Renderer::Submit([this, frameData]() {

        Renderer::Instance().GetRenderTargetPool().UpdatePools();

        mat4 viewProjMat = frameData.projectionMatrix * frameData.viewMatrix;

        globalUniformBufferContent->SetParameter("viewPos", frameData.cameraPos, ShaderFundamentalType::Float3);
        globalUniformBufferContent->SetParameter("viewMatrix", frameData.viewMatrix, ShaderFundamentalType::Mat4);
        globalUniformBufferContent->SetParameter("projectionMatrix", frameData.projectionMatrix, ShaderFundamentalType::Mat4);
        globalUniformBufferContent->SetParameter("viewProjectionMatrix", viewProjMat, ShaderFundamentalType::Mat4);

        GlobalRHICmdList->UpdateUniformBuffer(globalUniformBuffer.get(), globalUniformBufferContent->GetData());

        GlobalRHICmdList->SetStaticUniformBuffer("uGlobal", globalUniformBuffer.get());

        for (auto &renderable : frameData.renderables)
        {
            objectUniformBufferContent->SetParameter(renderable.index, "modelMatrix", renderable.modelMatrix, ShaderFundamentalType::Mat4);
            objectUniformBufferContent->SetParameter(renderable.index, "entityID", (uint32)renderable.entityID, ShaderFundamentalType::UInt);
        }

        GlobalRHICmdList->UpdateUniformBuffer(objectUniformBuffer.get(), objectUniformBufferContent->GetData());

        GlobalRHICmdList->SetStaticUniformBuffer("uMeshObject", objectUniformBuffer.get());

        // if (auto scenePanel = window->GetActivePanel<ScenePanel>())
        // {
        //     geometryPass->Render(frameData);
        //     shadowPass->Render(frameData);
        //     shadingPass->Render();
        //     postProcessingPass->Render();
        //     uiPass->Render();
        // }
        // EditorGUI::BeginFrame(); 
        // {
        //     GUIDockSpace dockspace("Hello DockSpace!");
        //     {
        //         for (auto &panel : window->panels)
        //         {
        //             panel->Render();
        //         }
        //         GUIWindow testWindow("Test");
        //         EditorGUI().Button("Hello", []() {
        //             Log::Info("Hello World!");
        //         });
        //     }
        // }
        // EditorGUI::EndFrame();
        
        // GlobalRHICmdList->Present();

        GlobalRHICmdList->FinalizeContext();

        GlobalRHICmdList->Submit();
    });

    // if (auto scenePanel = window->GetActivePanel<ScenePanel>())
    // {
    //     if (scenePanel->NeedResize())
    //     {
    //         Renderer::Submit([this, scenePanel]() mutable {
    //             scenePanel->UnregisterSceneTexture();
    //             geometryPass->Resize(scenePanel->size.x, scenePanel->size.y);
    //             shadingPass->Resize(scenePanel->size.x, scenePanel->size.y);
    //             postProcessingPass->Resize(scenePanel->size.x, scenePanel->size.y);
    //             uiPass->Resize(scenePanel->size.x, scenePanel->size.y);
    //             scenePanel->SetSceneTexture(uiPass->outputTexture);
    //         });
    //         editorCamera->SetViewportSize(scenePanel->size.x, scenePanel->size.y);
    //     }
    // }
}

}
#endif 