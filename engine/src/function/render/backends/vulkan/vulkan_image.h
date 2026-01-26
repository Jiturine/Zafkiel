#pragma once
#include "function/render/image.h"
#include "function/render/backends/vulkan/vulkan_device.h"

namespace Zafkiel
{

vk::Format ImageFormatToVulkanType(ImageFormat format);
ImageFormat VulkanFormatToImageFormat(vk::Format format);
vk::ImageUsageFlagBits ImageUsageToVulkanType(ImageUsage usage);
vk::ImageLayout ImageLayoutToVulkanType(ImageLayout layout);
vk::SampleCountFlagBits SampleCountToVulkanType(uint32_t sampleCount);
vk::ImageAspectFlags ImageFormatToVulkanImageAspect(ImageFormat format);

class VulkanImageBackend final : public ImageBackend
{
  public:
    VulkanImageBackend(uint32_t imageCount, std::vector<vk::raii::Image> images, std::vector<vk::raii::ImageView> imageViews, std::vector<vk::raii::DeviceMemory> memories);

    const std::vector<vk::raii::ImageView> &GetImageViews() const { return imageViews; }
    size_t GetImageCount() const { return imageCount; }
    const vk::raii::ImageView &GetImageView(size_t index) const 
    {
        if (index < imageCount) 
            return imageViews[index]; 
        else
            return imageViews[0];
    }
    const vk::raii::Image &GetImage(size_t index) const 
    { 
        if (index < imageCount) 
            return images[index]; 
        else
            return images[0];
    }
    const std::vector<vk::raii::Image> &GetImages() const { return images; }

  private:
    uint32_t imageCount;
    std::vector<vk::raii::Image> images;
    std::vector<vk::raii::ImageView> imageViews;
    std::vector<vk::raii::DeviceMemory> memories;
};
}
