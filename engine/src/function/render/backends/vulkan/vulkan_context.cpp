#include "vulkan_context.h"
#include "function/render/backends/vulkan/vulkan_image.h"
#include "function/render/backends/vulkan/vulkan_texture.h"
#include "imgui_impl_vulkan.h"
#include "vulkan/vulkan.hpp"
#include "vulkan_render_pass.h"
#include "vulkan_vertex_buffer.h"
#include "vulkan_index_buffer.h"
#include "vulkan_mesh.h"
#include "vulkan_graphics_shader.h"
#include "vulkan_material.h"
#include "vulkan_render_resource_template.h"
#include "vulkan_render_resource.h"
#include "vulkan_global_render_resource.h"
#include "vulkan_render_pass_resource.h"
#include "vulkan_object_render_resource.h"
#include <SDL3/SDL_vulkan.h>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_core.h>

namespace Zafkiel
{
VulkanContext::VulkanContext(const Window &window)
    : instance(nullptr), surface(nullptr), window(window.GetHandle())
{
    // 创建 vk::Instance
    vk::InstanceCreateInfo createInfo;
    vk::ApplicationInfo appInfo;
    appInfo.setApiVersion(VK_API_VERSION_1_4);

    createInfo.setPApplicationInfo(&appInfo);

    uint32_t extensionCount;
    const char *const *extensions = SDL_Vulkan_GetInstanceExtensions(&extensionCount);
    for (size_t i = 0; i < extensionCount; i++)
        this->extensions.push_back(extensions[i]);
    this->extensions.push_back(vk::EXTSurfaceMaintenance1ExtensionName);     // 
    this->extensions.push_back(vk::KHRGetSurfaceCapabilities2ExtensionName); // swapchain 同步扩展 

    std::vector<const char *> layers = {"VK_LAYER_KHRONOS_validation"};
    createInfo.setPpEnabledLayerNames(layers.data())
              .setEnabledLayerCount(layers.size())
              .setEnabledExtensionCount(this->extensions.size())
              .setPpEnabledExtensionNames(this->extensions.data());

    instance = context.createInstance(createInfo);

    // 从SDL获取surface
    VkSurfaceKHR cStyleSurface;
    bool success = SDL_Vulkan_CreateSurface(window.GetHandle(), *instance, nullptr, &cStyleSurface);
    if (!success)
    {
        Log::Error("Error when Create Surface: {}", SDL_GetError());
    }
    surface = vk::raii::SurfaceKHR { instance, cStyleSurface };

    // 选择物理设备
    auto devices = instance.enumeratePhysicalDevices();
    physicalDevice = CreateScope<VulkanPhysicalDevice>(devices[0], surface);

    // 创建逻辑设备
    device = CreateScope<VulkanDevice>(physicalDevice);

    // 创建描述符池
    descriptorManager = CreateScope<VulkanDescriptorManager>(device);

    // 创建交换链所需的主RenderPass
    RenderPassSpecification spec
    {
        .attachments =
        {
            {
                .format = ImageFormat::RGBA8,
                .initialLayout = ImageLayout::Undefined,
                .finalLayout = ImageLayout::PresentSrc,
                .samples = 1
            }
        },
        .subpasses =
        {
            {
                .attachmentRefs = {
                    {0, ImageLayout::ColorAttachment}
                },
                .type = PipelineType::Graphics
            }
        }
    };
    auto renderPassBackend = CreateScope<VulkanRenderPassBackend>(spec, device);
    mainRenderPass = CreateScope<RenderPass>(spec, std::move(renderPassBackend));

    auto capabilities = physicalDevice->GetHandle().getSurfaceCapabilitiesKHR(surface);
    isSurfaceSizeFixed = capabilities.minImageExtent.width == capabilities.maxImageExtent.width
                           && capabilities.minImageExtent.height == capabilities.maxImageExtent.height;
    // 创建交换链
    VulkanSwapchainSpecification swapchainSpecification
    {
        .device = device,
        .physicalDevice = physicalDevice,
        .renderPass = mainRenderPass,
        .surface = surface,
        .width = window.GetWidth(),
        .height = window.GetHeight()
    };
    swapchain = CreateScope<VulkanSwapchain>(swapchainSpecification);

    // 创建命令池
    commandManager = CreateScope<VulkanCommandManager>(device, physicalDevice);

    // 创建flight
    for (size_t i = 0; i < flightCount; i++)
    {
        framesInFlight.emplace_back(CreateScope<VulkanFrameInFlight>(commandManager, device));
    }
    for (size_t i = 0; i < swapchain->GetFrameCount(); i++)
    {
        renderFinishedSem.emplace_back(device->GetHandle().createSemaphore({}));
    }
    curFrame = 0;

    device->GetHandle().waitIdle();
}

void VulkanContext::Resize(uint32_t width, uint32_t height)
{
    ResizeSwapchain(width, height);
}

Scope<GraphicsPipeline> VulkanContext::CreateGraphicsPipeline(const GraphicsPipelineSpecification &spec) const
{
    return VulkanGraphicsPipelineFactory::Create(spec, device);
}

Scope<RenderPass> VulkanContext::CreateRenderPass(const RenderPassSpecification &spec) const
{
    return VulkanRenderPassFactory::Create(spec, device);
}

Scope<GraphicsShader> VulkanContext::CreateGraphicsShader(const Path &filePath) const
{
    return VulkanGraphicsShaderFactory::Create(filePath, device, descriptorManager);
}

Scope<Material> VulkanContext::CreateMaterial(const MaterialSpecification &spec) const
{
    return VulkanMaterialFactory::Create(spec, device, physicalDevice, descriptorManager, flightCount);
}

Scope<VertexBuffer> VulkanContext::CreateVertexBuffer(const float *vertices, uint32_t size) const
{
    return VulkanVertexBufferFactory::Create(vertices, size, device, physicalDevice, commandManager);
}
Scope<IndexBuffer> VulkanContext::CreateIndexBuffer(const uint32_t *indices, uint32_t count) const
{
    return VulkanIndexBufferFactory::Create(indices, count, device, physicalDevice, commandManager);
}
Scope<Mesh> VulkanContext::CreateMesh(const std::vector<MeshVertex> &vertices, const std::vector<uint32_t> &indices) const
{
    return VulkanMeshFactory::Create(vertices, indices, device, physicalDevice, commandManager);
}
Scope<FrameBuffer> VulkanContext::CreateFrameBuffer(const FrameBufferSpecification &spec) const
{
    return VulkanFrameBufferFactory::Create(spec, device);
}
Scope<Image> VulkanContext::CreateImage(const ImageSpecification &spec) const
{
    return VulkanImageFactory::Create(spec, device, physicalDevice, flightCount);
}
Scope<Texture2D> VulkanContext::CreateTexture2D(const Texture2DSpecification &spec) const
{
    return VulkanTexture2DFactory::Create(spec, device, physicalDevice, commandManager, flightCount);
}

Scope<Texture2D> VulkanContext::CreateTexture2D(const Texture2DSpecification &spec, Buffer buffer) const
{
    return VulkanTexture2DFactory::Create(spec, buffer, device, physicalDevice, commandManager, flightCount);
}

Scope<RenderResourceTemplate> VulkanContext::CreateRenderResourceTemplate(const Observer<RenderResourceSchema> schema) const
{
    return VulkanRenderResourceTemplateFactory::Create(schema, device);
}

Scope<RenderResource> VulkanContext::CreateRenderResource(const Observer<RenderResourceTemplate> renderResourceTemplate) const
{
    return VulkanRenderResourceFactory::Create(renderResourceTemplate, device, physicalDevice, descriptorManager, flightCount);
}

Scope<GlobalRenderResource> VulkanContext::CreateGlobalRenderResource(const Path &path) const 
{
    return VulkanGlobalRenderResourceFactory::Create(path, device, physicalDevice, descriptorManager, flightCount);
}

Scope<RenderPassResource> VulkanContext::CreateRenderPassResource(const Path &path) const
{
    return VulkanRenderPassResourceFactory::Create(path, device, physicalDevice, descriptorManager, flightCount);
}

Scope<ObjectRenderResource> VulkanContext::CreateObjectRenderResource(const Path &path) const
{
    return VulkanObjectRenderResourceFactory::Create(path, device, physicalDevice, descriptorManager, flightCount);
}

std::vector<vk::DescriptorSet> VulkanContext::RegisterImGuiTexture2D(Observer<Texture2D> texture)
{
    device->GetHandle().waitIdle();
    auto key = *texture->GetImage()->GetBackend().As<VulkanImageBackend>()->GetImage(0);

    std::vector<vk::DescriptorSet> result;
    auto imageBackend = texture->GetImage()->GetBackend().As<VulkanImageBackend>();

    // 确保所有图像副本都处于正确的布局
    size_t imageCount = imageBackend->GetImageCount();
    for (size_t i = 0; i < imageCount; i++)
    {
        // 检查并转换图像布局到ShaderReadOnly
        auto cmdBuf = commandManager->CreateOneCommandBuffer();
        vk::CommandBufferBeginInfo beginInfo;
        beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
        cmdBuf.begin(beginInfo);

        vk::ImageMemoryBarrier barrier;
        barrier.setOldLayout(vk::ImageLayout::eUndefined)
              .setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
              .setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
              .setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED)
              .setImage(*imageBackend->GetImage(i))
              .setSubresourceRange({
                  vk::ImageAspectFlagBits::eColor,
                  0, 1, 0, 1
              })
              .setSrcAccessMask(vk::AccessFlagBits::eNone)
              .setDstAccessMask(vk::AccessFlagBits::eShaderRead);

        cmdBuf.pipelineBarrier(
            vk::PipelineStageFlagBits::eTopOfPipe,
            vk::PipelineStageFlagBits::eFragmentShader,
            vk::DependencyFlags{},
            {}, {}, barrier
        );

        cmdBuf.end();

        vk::SubmitInfo submitInfo;
        submitInfo.setCommandBuffers(*cmdBuf);
        device->GetGraphicsQueue().submit(submitInfo);
        device->GetHandle().waitIdle();
    }

    for (auto &imageView : imageBackend->GetImageViews())
    {
        auto descriptorSet = ImGui_ImplVulkan_AddTexture(*texture->GetBackend().As<VulkanTexture2DBackend>()->GetSampler(), *imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        result.push_back(descriptorSet);
    }

    registeredImGuiTextures[key] = result;
    return result;
}

void VulkanContext::UnregisterImGuiTexture2D(Observer<Texture2D> texture)
{
    device->GetHandle().waitIdle();
    auto key = *texture->GetImage()->GetBackend().As<VulkanImageBackend>()->GetImage(0);
    for (VkDescriptorSet set : registeredImGuiTextures[key])
    {
        ImGui_ImplVulkan_RemoveTexture(set);
    }
}

void VulkanContext::RenderToScreen(std::function<void()> renderFunc)
{
    auto &frame = framesInFlight[curFrame];
    vk::ClearValue clearValue;
    clearValue.setColor(vk::ClearColorValue(std::array<float, 4>{0.1, 0.1, 0.1, 1}));
    vk::RenderPassBeginInfo renderPassBegin;
    renderPassBegin.setRenderPass(mainRenderPass->GetBackend().As<VulkanRenderPassBackend>()->GetHandle())
                   .setFramebuffer(*swapchain->GetFrameBuffer(curImageIndex))
                   .setClearValues(clearValue)
                   .setRenderArea(vk::Rect2D({}, {swapchain->GetWidth(), swapchain->GetHeight()}));
    frame->cmdBuf.beginRenderPass(renderPassBegin, vk::SubpassContents::eInline);

    renderFunc();

    frame->cmdBuf.endRenderPass();
}

void VulkanContext::InvalidateSwapchain()
{
    device->GetHandle().waitIdle(); // 确保 GPU 完全空闲

    if (isSurfaceSizeFixed) // Some cases on Linux
    {
        device->GetHandle().waitIdle();
        swapchain = nullptr;
        surface = nullptr;

        VkSurfaceKHR cStyleSurface;
        SDL_Vulkan_CreateSurface(window, *instance, nullptr, &cStyleSurface);
        surface = vk::raii::SurfaceKHR { instance, cStyleSurface };
        auto capabilities = physicalDevice->GetHandle().getSurfaceCapabilitiesKHR(surface);

        VulkanSwapchainSpecification swapchainSpecification
        {
            .device = device,
            .physicalDevice = physicalDevice,
            .renderPass = mainRenderPass,
            .surface = surface,
            .width = capabilities.currentExtent.width,
            .height = capabilities.currentExtent.height
        };
        swapchain = CreateScope<VulkanSwapchain>(swapchainSpecification);
    }
    else
    {
        swapchain->Invalidate();
    }
}

void VulkanContext::ResizeSwapchain(uint32_t width, uint32_t height)
{
    if (isSurfaceSizeFixed) // Linux
    {
        device->GetHandle().waitIdle();
        swapchain = nullptr;
        surface = nullptr;

        VkSurfaceKHR cStyleSurface;
        SDL_Vulkan_CreateSurface(window, *instance, nullptr, &cStyleSurface);
        surface = vk::raii::SurfaceKHR { instance, cStyleSurface };
        auto capabilities = physicalDevice->GetHandle().getSurfaceCapabilitiesKHR(surface);

        VulkanSwapchainSpecification swapchainSpecification
        {
            .device = device,
            .physicalDevice = physicalDevice,
            .renderPass = mainRenderPass,
            .surface = surface,
            .width = capabilities.currentExtent.width,
            .height = capabilities.currentExtent.height
        };
        swapchain = CreateScope<VulkanSwapchain>(swapchainSpecification);
    }
    else
    {
        swapchain->Resize(width, height);
    }
}

void VulkanContext::BeginFrame()
{
    auto& frame = framesInFlight[curFrame];
    try {
        if (device->GetHandle().waitForFences(*frame->fence, true, std::numeric_limits<uint64_t>::max()) != vk::Result::eSuccess)
        {
            Log::Error("Wait for Fence Failed");
        }
    } catch (vk::DeviceLostError &e)
    {
        Log::Error("Device Lost Error: {}", e.what());
        return;
    }

    uint32_t imageIndex = 0;
    try
    {
        auto resultValue = swapchain->GetHandle().acquireNextImage(std::numeric_limits<uint64_t>::max(), *frame->imageAvailableSem, nullptr);
        if (resultValue.result == vk::Result::eSuboptimalKHR)
            InvalidateSwapchain();
        imageIndex = resultValue.value;
    }
    catch (const vk::OutOfDateKHRError&)
    {
        InvalidateSwapchain();
        return;
    }
    curImageIndex = imageIndex;

    device->GetHandle().resetFences(*frame->fence);

    frame->cmdBuf.reset();

    vk::CommandBufferBeginInfo beginInfo;
    beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    frame->cmdBuf.begin(beginInfo);
}

void VulkanContext::EndFrame()
{
    auto &frame = framesInFlight[curFrame];
    frame->cmdBuf.end();

    vk::SubmitInfo submit;
    vk::PipelineStageFlags flags = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    submit.setCommandBuffers(*frame->cmdBuf)
          .setWaitSemaphores(*frame->imageAvailableSem)
          .setWaitDstStageMask(flags)
          .setSignalSemaphores(*frame->renderFinishedSem);
    try
    {
        device->GetGraphicsQueue().submit(submit, nullptr);
    }
    catch (const vk::SystemError& e)
    {
        Log::Error("Submit failed: {}", e.what());
        InvalidateSwapchain();
        return;
    }
    vk::StructureChain<vk::PresentInfoKHR, vk::SwapchainPresentFenceInfoEXT> presentChain;
    presentChain.get().setWaitSemaphores(*frame->renderFinishedSem)
                      .setSwapchains(*swapchain->GetHandle())
                      .setImageIndices(curImageIndex);
    presentChain.get<vk::SwapchainPresentFenceInfoEXT>().setSwapchainCount(1)
                                                        .setFences(*frame->fence);
    try
    {
        auto result = device->GetPresentQueue().presentKHR(presentChain.get());
        if (result == vk::Result::eSuboptimalKHR)
            InvalidateSwapchain();
    }
    catch (vk::OutOfDateKHRError&)
    {
        InvalidateSwapchain();
    }
    curFrame = (curFrame + 1) % flightCount;
}

}
