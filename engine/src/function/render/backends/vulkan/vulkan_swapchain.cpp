#include "vulkan_swapchain.h"
#include "vulkan_device.h"
#include "vulkan_image.h"

namespace Zafkiel 
{
VulkanSwapchain::VulkanSwapchain(const VulkanSwapchainSpecification& spec)
    : width(spec.width), height(spec.height), swapchain(nullptr), device(spec.device), physicalDevice(spec.physicalDevice), surface(spec.surface), renderPass(spec.renderPass)
{
    auto &surface = spec.surface;
    auto formats = physicalDevice->physicalDevice.getSurfaceFormatsKHR(surface);
    surfaceFormat = formats[0];
    for (const auto &format : formats)
    {
        if (format.format == vk::Format::eR8G8B8A8Unorm && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
        {
            surfaceFormat = format;
            break;
        }
    }
    auto capabilities = physicalDevice->physicalDevice.getSurfaceCapabilitiesKHR(surface);
    
    if (capabilities.minImageExtent == capabilities.maxImageExtent)
    {
        width = capabilities.currentExtent.width;
        height = capabilities.currentExtent.height;
    }

    frameCount = std::clamp(3u, capabilities.minImageCount, capabilities.maxImageCount);

    auto presentModes = physicalDevice->physicalDevice.getSurfacePresentModesKHR(surface);
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
    device->GetHandle().waitIdle();
    frameBuffers.clear();
    imageViews.clear();
    images.clear();
    swapchain = nullptr;

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

    if (physicalDevice->queueFamilyIndices.graphicsQueueIndex == physicalDevice->queueFamilyIndices.presentQueueIndex)
        createInfo.setQueueFamilyIndices(physicalDevice->queueFamilyIndices.graphicsQueueIndex.value())
                  .setImageSharingMode(vk::SharingMode::eExclusive);
    else
    {
        std::array<uint32_t, 2> indices = {physicalDevice->queueFamilyIndices.graphicsQueueIndex.value(), physicalDevice->queueFamilyIndices.presentQueueIndex.value()};
        createInfo.setQueueFamilyIndices(indices)
                  .setImageSharingMode(vk::SharingMode::eConcurrent);
    }
    swapchain = device->GetHandle().createSwapchainKHR(createInfo);

    images = swapchain.getImages();

    for (auto &image : images)
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

        imageViews.emplace_back( device->GetHandle().createImageView(createInfo));
    }

    for (auto &imageView : imageViews)
    {
        vk::FramebufferCreateInfo createInfo;
        createInfo.setAttachments(*imageView)
                  .setWidth(width)
                  .setHeight(height)
                  .setRenderPass(renderPass->GetBackend().As<VulkanRenderPassBackend>()->GetHandle())
                  .setLayers(1);
        frameBuffers.emplace_back(device->GetHandle().createFramebuffer(createInfo));
    }
}

void VulkanSwapchain::Resize(uint32_t width, uint32_t height)
{
    this->width = width;
    this->height = height;
    
    Invalidate();
}

}