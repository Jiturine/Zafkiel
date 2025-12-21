#include "renderer.h"
#include "backends/opengl/opengl_context.h"
#include "backends/opengl/opengl_command.h"
#include "backends/opengl/opengl_image.h"
#include "backends/vulkan/vulkan_context.h"
#include "backends/vulkan/vulkan_command.h"

#include <imgui.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_vulkan.h>

namespace Zafkiel 
{
Renderer::Renderer(GraphicsAPI API, const Window &window)
{
    Maths::SetAPI(API);
    Application::StartRenderThread([this, API, &window](){
        if (API == GraphicsAPI::OpenGL)
        {
            graphicsContext = CreateScope<OpenGLContext>(window);
            RenderCommand::instance = CreateScope<OpenGLCommand>();
        }
        else if (API == GraphicsAPI::Vulkan)
        {
            graphicsContext = CreateScope<VulkanContext>(window);
            RenderCommand::instance = CreateScope<VulkanCommand>();
        }
        InitImGui(window.GetHandle());
        glslang::InitializeProcess();
        LoadBuiltInMaterialTemplates();
    });
}

Renderer::~Renderer() 
{
    
}

Observer<Mesh> Renderer::GetMeshImpl(const Ref<MeshAsset> &meshAsset)
{
    if (!meshes.contains(meshAsset->handle))
    {
        meshes[meshAsset->handle] = graphicsContext->CreateMesh(meshAsset->GetVertices(), meshAsset->GetIndices());
    }
    return meshes.at(meshAsset->handle);
}

Observer<Material> Renderer::GetMaterialImpl(const Ref<MaterialAsset> &materialAsset)
{
    if (!materials.contains(materialAsset->handle))
    {
        MaterialSpecification spec 
        {
            .name = materialAsset->GetName(),
            .shaderFamily = materialAsset->GetShaderFamily(),
            .materialTemplate = builtInMaterialTemplates[materialAsset->GetShaderFamily()]
        };
        auto material = graphicsContext->CreateMaterial(spec);
        for (auto &[paramName, param] : material->GetRenderResource()->GetParameters())
        {
            if (param.type->GetCategory() == ShaderReflection::ResourceTypeCategory::UniformBlock)
            {
                param = RenderResourceParameter(param.type->As<ShaderReflection::UniformBlock>());
                param.uniformBuffer = materialAsset->GetParameters().at(paramName).uniformBuffer.Clone();
            }
            else if (param.type->GetCategory() == ShaderReflection::ResourceTypeCategory::SampledImage)
            {
                auto texture2DAsset = AssetManager::GetAsset(materialAsset->GetParameters().at(paramName).handle).As<Texture2DAsset>();
                param.texture2D = GetTexture2DImpl(texture2DAsset); 
                // TODO: 优化
                material->GetRenderResource()->GetBackend()->SetTexture2D(paramName, param.texture2D);
            }
        }
        materials[materialAsset->handle] = std::move(material);
    }
    return materials.at(materialAsset->handle);
}

Observer<Texture2D> Renderer::GetTexture2DImpl(const Ref<Texture2DAsset> &texture2DAsset)
{
    if (!texture2Ds.contains(texture2DAsset->handle))
    {
        texture2Ds[texture2DAsset->handle] = graphicsContext->CreateTexture2D(texture2DAsset->GetSpecification(), texture2DAsset->GetBuffer());
    }
    return texture2Ds.at(texture2DAsset->handle);
}

void Renderer::LoadBuiltInMaterialTemplates()
{
    builtInMaterialSchemas[ShaderFamily::BlinnPhong] = CreateScope<RenderResourceSchema>("assets/shaders/schema/blinn_phong.zss");
    builtInMaterialTemplates[ShaderFamily::BlinnPhong] = graphicsContext->CreateRenderResourceTemplate(builtInMaterialSchemas[ShaderFamily::BlinnPhong]);
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

    if (graphicsContext->GetAPI() == GraphicsAPI::OpenGL)
    {
        ImGui_ImplSDL3_InitForOpenGL(window, graphicsContext.As<OpenGLContext>().GetHandle());
        ImGui_ImplOpenGL3_Init("#version 450");
    }
    else 
    {
        ImGui_ImplSDL3_InitForVulkan(window);
        ImGui_ImplVulkan_InitInfo init_info = {};
        auto &context = graphicsContext.As<VulkanContext>();
        init_info.ApiVersion = VK_API_VERSION_1_4;
        init_info.Instance = *context.GetInstance();
        init_info.PhysicalDevice = *context.GetPhysicalDevice()->GetHandle();
        init_info.Device = *context.GetDevice()->GetHandle();
        init_info.QueueFamily = context.GetPhysicalDevice()->GetGraphicsQueueIndex();
        init_info.Queue = *context.GetDevice()->GetGraphicsQueue();
        init_info.PipelineCache = nullptr;
        init_info.DescriptorPool = *context.GetDescriptorManager()->GetDescriptorPool();
        init_info.PipelineInfoMain.RenderPass = *context.GetMainRenderPass()->GetBackend().As<VulkanRenderPassBackend>()->GetHandle();
        init_info.PipelineInfoMain.Subpass = 0;
        init_info.MinImageCount = 3;
        init_info.ImageCount = 3;
        init_info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        ImGui_ImplVulkan_Init(&init_info);
    }
}

void Renderer::DestroyImGui() 
{
    if (graphicsContext->GetAPI() == GraphicsAPI::OpenGL)
        ImGui_ImplOpenGL3_Shutdown();
    else if (graphicsContext->GetAPI() == GraphicsAPI::Vulkan)
    {
        graphicsContext.As<VulkanContext>().GetDevice()->GetHandle().waitIdle();
        ImGui_ImplVulkan_Shutdown();
    }
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}

std::vector<ImTextureRef> Renderer::RegisterImGuiTextureImpl(Observer<Texture2D> texture)
{
    if (graphicsContext->GetAPI() == GraphicsAPI::Vulkan)
    {
        auto descriptorSets = graphicsContext.As<VulkanContext>().RegisterImGuiTexture2D(texture);
        std::vector<ImTextureRef> result;
        for (auto &descriptorSet : descriptorSets)
        {
            result.push_back((ImTextureRef)descriptorSet);
        }
        return result;
    }
    else if (graphicsContext->GetAPI() == GraphicsAPI::OpenGL)
    {
        return { texture->GetImage()->GetBackend().As<OpenGLImageBackend>()->GetRendererID() };
    }
    else return {};
}

void Renderer::UnregisterImGuiTextureImpl(Observer<Texture2D> texture)
{
    if (graphicsContext->GetAPI() == GraphicsAPI::Vulkan)
    {
        graphicsContext.As<VulkanContext>().UnregisterImGuiTexture2D(texture);
    }
}

void Renderer::BeginFrameImpl()
{
    graphicsContext->BeginFrame();
    RenderCommand::instance->BeginFrame(graphicsContext);
}

void Renderer::EndFrameImpl()
{
    graphicsContext->EndFrame();
}

}