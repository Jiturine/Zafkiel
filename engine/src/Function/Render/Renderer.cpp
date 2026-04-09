#include "Function/Render/Renderer.h"
#include "Function/RHI/RHI.h"
#include "Function/RHI/Backends/OpenGL/OpenGLRHI.h"
#include "Function/RHI/Backends/Vulkan/VulkanRHI.h"

#include <imgui.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_vulkan.h>

#include "Resource/Texture2DAsset.h"
#include "Resource/MaterialAsset.h"
#include "Resource/MeshAsset.h"
#include "Resource/FontAsset.h"

#include "Function/RHI/ShaderReflection.h"

namespace Zafkiel 
{
Renderer::Renderer(GraphicsAPI API, Window &window)
{
    Maths::SetAPI(API);
    Application::StartRenderThread([this, API, &window](){
        if (API == GraphicsAPI::OpenGL)
        {
            GlobalRHI = CreateScope<OpenGLRHI>(window);
        }
        else if (API == GraphicsAPI::Vulkan)
        {
            GlobalRHI = CreateScope<VulkanRHI>(window);
        }
        GlobalRHICmdList = CreateScope<RHICommandListImmediate>(GlobalRHI->GetGraphicsContext(), *GlobalRHI.get());

        InitImGui(window.GetHandle());

        renderTargetPool = CreateScope<RenderTargetPool>();
    });
}

Renderer::~Renderer() 
{
    
}

void Renderer::Init(GraphicsAPI API, Window &window)
{
    instance = new Renderer(API, window);
}

void Renderer::Destroy()
{
    Submit([](){
        instance->DestroyImGui();
    });
    Application::KickRenderThread();
    Application::WaitRenderThread();
    Application::StopRenderThread();
    delete instance;
    instance = nullptr;

    GlobalRHICmdList = nullptr;
    GlobalRHI = nullptr;
}

void Renderer::InitImGui(SDL_Window* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    ImGuiStyle &style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);
    style.FontScaleDpi = main_scale;
    io.ConfigDpiScaleFonts = true;
    io.ConfigDpiScaleViewports = true;

    ImGui::StyleColorsDark();

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
    io.Fonts->AddFontFromFileTTF("assets/fonts/HarmonyOS_Sans_SC/HarmonyOS_Sans_SC_Regular.ttf");

    if (GlobalRHI->GetAPI() == GraphicsAPI::OpenGL)
    {
        OpenGLRHI *glRHI = static_cast<OpenGLRHI *>(GlobalRHI.get());
        ImGui_ImplSDL3_InitForOpenGL(window, glRHI->GetGLContext());
        ImGui_ImplOpenGL3_Init("#version 450");
    }
    else 
    {
        VulkanRHI *vulkanRHI = static_cast<VulkanRHI *>(GlobalRHI.get());
        VulkanDevice &device = vulkanRHI->GetDevice();

        RenderTargetDesc renderTargetDesc 
        {
            .colorAttachmentDescs
            {
                {
                    .format = ImageFormat::RGBA8,
                    .sampleCount = 1,
                },
            }
        };
        VulkanRenderTargetInfo renderTargetInfo(renderTargetDesc);

        auto renderPass = device.GetRenderPassManager().GetOrCreateRenderPass(renderTargetInfo);

        ImGui_ImplSDL3_InitForVulkan(window);
        ImGui_ImplVulkan_InitInfo init_info{};

        init_info.ApiVersion = VK_API_VERSION_1_4;
        init_info.Instance = *vulkanRHI->GetInstance();
        init_info.PhysicalDevice = *device.GetPhysicalHandle();
        init_info.Device = *device.GetHandle();
        init_info.QueueFamily = device.GetGraphicsQueue().GetQueueFamilyIndex();
        init_info.Queue = *device.GetGraphicsQueue().GetHandle();
        init_info.PipelineCache = nullptr;
        init_info.DescriptorPool = *device.GetDescriptorManager().GetDescriptorPool();
        init_info.PipelineInfoMain.RenderPass = *renderPass->GetHandle();
        init_info.PipelineInfoMain.Subpass = 0;
        init_info.MinImageCount = 3;
        init_info.ImageCount = 3;
        init_info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        ImGui_ImplVulkan_Init(&init_info);
    }
}

void Renderer::DestroyImGui() 
{
    if (GlobalRHI->GetAPI() == GraphicsAPI::OpenGL)
        ImGui_ImplOpenGL3_Shutdown();
    else if (GlobalRHI->GetAPI() == GraphicsAPI::Vulkan)
    {
        // graphicsContext.As<VulkanContext>().GetDevice()->GetHandle().waitIdle();
        ImGui_ImplVulkan_Shutdown();
    }
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}



Ref<Mesh> Renderer::GetOrCreateMesh(AssetHandle meshAssetHandle)
{
    if (meshes.contains(meshAssetHandle))
    {
        return meshes[meshAssetHandle];
    }

    Ref<Mesh> mesh = CreateRef<Mesh>(meshAssetHandle);

    meshes[meshAssetHandle] = mesh;

    return mesh;
}

Ref<Material> Renderer::GetOrCreateMaterial(AssetHandle materialAssetHandle, const MaterialDesc &desc)
{
    if (materials.contains(materialAssetHandle))
    {
        return materials[materialAssetHandle];
    }

    Ref<Material> material = CreateRef<Material>(materialAssetHandle, desc);

    materials[materialAssetHandle] = material;

    return material;
}

}
