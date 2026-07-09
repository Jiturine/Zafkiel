#include "Function/RHI/Backends/Vulkan/VulkanSwapchain.h"
#include "Function/RHI/Backends/Vulkan/VulkanDevice.h"
#include "Function/RHI/Backends/Vulkan/VulkanQueue.h"
#include "Function/RHI/Backends/Vulkan/VulkanTexture.h"
#include "Platform/PlatformWindow/PlatformWindow.h"
#include <SDL3/SDL_vulkan.h>

namespace Zafkiel 
{
  
VulkanSwapchainTexture::VulkanSwapchainTexture(uint32 width, uint32 height, vk::Image image, vk::SurfaceFormatKHR surfaceFormat, VulkanDevice &device)
    : VulkanTextureBase(RHITextureDesc {
        .width = width,
        .height = height,
        .format = VulkanFormatToImageFormat(surfaceFormat.format),
        .initialLayout = ImageLayout::PresentSrc,
        .sampleCount = 1,
    }), imageView(nullptr), image(image)
{
    vk::ImageViewCreateInfo createInfo;
    vk::ComponentMapping mapping;
    vk::ImageSubresourceRange range;
    range.setBaseMipLevel(0)
            .setLevelCount(1)
            .setBaseArrayLayer(0)
            .setLayerCount(1)
            .setAspectMask(vk::ImageAspectFlagBits::eColor);

    createInfo.setImage(image)
                .setViewType(vk::ImageViewType::e2D)
                .setComponents(mapping)
                .setFormat(surfaceFormat.format)
                .setSubresourceRange(range);
    
    imageView = device.GetHandle().createImageView(createInfo);

    device.GetGraphicsContext()->ImageMemoryBarrier(this, 
        vk::AccessFlagBits::eNone, vk::AccessFlagBits::eMemoryRead,
        vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR,
        vk::PipelineStageFlagBits::eBottomOfPipe, vk::PipelineStageFlagBits::eBottomOfPipe);
}

VulkanSwapchain::VulkanSwapchain(PlatformWindow *window, vk::raii::Instance &instance, VulkanDevice &device, uint32 width, uint32 height)
    : width(width), height(height), handle(nullptr), device(device), surface(nullptr)
{
    VkSurfaceKHR cStyleSurface;
    bool success = SDL_Vulkan_CreateSurface(window->GetHandle(), *instance, nullptr, &cStyleSurface);
    if (!success)
    {
        Log::Error("Error when Create Surface: {}", SDL_GetError());
    }
    surface = vk::raii::SurfaceKHR(instance, cStyleSurface);
    
    auto formats = device.GetPhysicalHandle().getSurfaceFormatsKHR(surface);
    surfaceFormat = formats[0];
    for (const auto &format : formats)
    {
        if (format.format == vk::Format::eR8G8B8A8Unorm && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
        {
            surfaceFormat = format;
            break;
        }
    }
    auto capabilities = device.GetPhysicalHandle().getSurfaceCapabilitiesKHR(surface);
    
    if (capabilities.minImageExtent == capabilities.maxImageExtent)
    {
        width = capabilities.currentExtent.width;
        height = capabilities.currentExtent.height;
    }

    frameCount = std::clamp(3u, capabilities.minImageCount, capabilities.maxImageCount);

    auto presentModes = device.GetPhysicalHandle().getSurfacePresentModesKHR(surface);
    presentMode = vk::PresentModeKHR::eFifo;
    for (const auto &present : presentModes)
    {
        if (present == vk::PresentModeKHR::eMailbox)
        {
            presentMode = present;
            break;
        }
    }
    Invalidate();
}

void VulkanSwapchain::Invalidate()
{
    device.GetHandle().waitIdle();

    vk::SwapchainCreateInfoKHR createInfo;
    createInfo.setClipped(true)
              .setImageArrayLayers(1)
              .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
              .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque) // 不融混
              .setSurface(surface)
              .setImageColorSpace(surfaceFormat.colorSpace)
              .setImageFormat(surfaceFormat.format)
              .setImageExtent({width, height})
              .setMinImageCount(frameCount)
              .setPresentMode(presentMode);

    auto queueFamilyIndex = device.graphicsQueue->GetQueueFamilyIndex();
    createInfo.setQueueFamilyIndices(queueFamilyIndex)
              .setImageSharingMode(vk::SharingMode::eExclusive);

    if (*handle)
    {
        createInfo.setOldSwapchain(*handle);
    }

    handle = device.GetHandle().createSwapchainKHR(createInfo);

    auto images = handle.getImages();

    swapchainTextures.clear();
    for (auto &image : images)
    {
        auto swapchainTexture = CreateRef<VulkanSwapchainTexture>(width, height, image, surfaceFormat, device);

        swapchainTextures.push_back(swapchainTexture);
    }

    imageAvailableSemaphores.clear();
    renderFinishedSemaphores.clear();
    fences.clear();
    for (uint32 i = 0; i < frameCount; i++)
    {
        imageAvailableSemaphores.push_back(device.GetHandle().createSemaphore({}));
        renderFinishedSemaphores.push_back(device.GetHandle().createSemaphore({}));
        
        vk::FenceCreateInfo fenceCreateInfo;
        fenceCreateInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);
        fences.push_back(device.GetHandle().createFence(fenceCreateInfo));
    }
}

void VulkanSwapchain::Resize(uint32 width, uint32 height)
{
    this->width = width;
    this->height = height;
    Invalidate();
}

uint32 VulkanSwapchain::AcquireNextImageIndex()
{
    uint32 prevSemaphoreIndex = semaphoreIndex;
    try
    {
        semaphoreIndex = (semaphoreIndex + 1) % imageAvailableSemaphores.size();

        auto waitResult = device.GetHandle().waitForFences(*fences[semaphoreIndex], true, std::numeric_limits<uint64_t>::max());
        if (waitResult != vk::Result::eSuccess)
        {
            Log::Error("Wait fence failed");
        }

        device.GetHandle().resetFences(*fences[semaphoreIndex]);

        auto resultValue = handle.acquireNextImage(std::numeric_limits<uint64_t>::max(), *imageAvailableSemaphores[semaphoreIndex], nullptr);
        if (resultValue.result == vk::Result::eSuboptimalKHR)
            Invalidate();

        curImageIndex = resultValue.value;

        return curImageIndex;
    }
    catch (const vk::OutOfDateKHRError&)
    {
        semaphoreIndex = prevSemaphoreIndex;
        Invalidate();
        return 0;
    }
}

VulkanSwapchainTexture *VulkanSwapchain::GetCurrentAvailableTexture()
{
    uint32 index = AcquireNextImageIndex();
    return swapchainTextures[index].get();
}

void VulkanSwapchain::Present()
{
    auto payload = device.GetGraphicsContext()->GetPayload(VulkanPayloadPhase::Execute);
    payload->waitSemaphores.push_back(imageAvailableSemaphores[semaphoreIndex]);
    payload->waitStages.push_back(vk::PipelineStageFlagBits::eColorAttachmentOutput);
    payload->signalSemaphores.push_back(renderFinishedSemaphores[semaphoreIndex]);

    device.GetGraphicsContext()->Submit();
    
    vk::StructureChain<vk::PresentInfoKHR, vk::SwapchainPresentFenceInfoEXT> presentChain;
    presentChain.get().setWaitSemaphores(*renderFinishedSemaphores[semaphoreIndex])
                      .setSwapchains(*handle)
                      .setImageIndices(curImageIndex);
    presentChain.get<vk::SwapchainPresentFenceInfoEXT>().setSwapchainCount(1)
                                                        .setFences(*fences[semaphoreIndex]);
    try
    {
        auto result = device.GetGraphicsQueue().GetHandle().presentKHR(presentChain.get());
        if (result == vk::Result::eSuboptimalKHR)
            Invalidate();
    }
    catch (vk::OutOfDateKHRError&)
    {
        Invalidate();
    }
}

}