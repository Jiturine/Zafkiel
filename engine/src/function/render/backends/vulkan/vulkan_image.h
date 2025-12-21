#pragma once
#include "function/render/image.h"
#include "vulkan_device.h"

namespace Zafkiel
{

vk::Format ImageFormatToVulkanType(ImageFormat format);
ImageFormat VulkanFormatToImageFormat(vk::Format format);
vk::ImageUsageFlagBits ImageUsageToVulkanType(ImageUsage usage);
vk::ImageLayout ImageLayoutToVulkanType(ImageLayout layout);
vk::SampleCountFlagBits SampleCountToVulkanType(uint32_t sampleCount);

class VulkanImageBackend final : public ImageBackend
{
  public:
    VulkanImageBackend(const ImageSpecification &spec, const Scope<VulkanDevice> &device, const Scope<VulkanPhysicalDevice> &physicalDevice, uint32_t frameCount);

    const std::vector<vk::raii::ImageView> &GetImageViews() const { return imageViews; }
    size_t GetImageCount() const { return imageCount; }
    const vk::raii::ImageView &GetImageView(size_t index) const 
    {
        if (index < imageViews.size()) 
            return imageViews[index]; 
        else
            return imageViews[0];
    }
    const vk::raii::Image &GetImage(size_t index) const 
    { 
        if (index < images.size()) 
            return images[index]; 
        else
            return images[0];
    }
    const std::vector<vk::raii::Image> &GetImages() const { return images; }

    void Resize(uint32_t width, uint32_t height) override;
    void Invalidate(uint32_t width, uint32_t height);

  private:
    uint32_t imageCount;
    vk::ImageUsageFlags usages;
    vk::Format format;
    ImageFormat imageFormat;
    vk::SampleCountFlagBits sampleCount;
    std::vector<vk::raii::Image> images;
    std::vector<vk::raii::ImageView> imageViews;
    std::vector<vk::raii::DeviceMemory> memories;
    const Scope<VulkanDevice> &device;
    const Scope<VulkanPhysicalDevice> &physicalDevice;
};

class VulkanImageFactory
{
  public:
    static Scope<Image> Create(const ImageSpecification &spec, const Scope<VulkanDevice> &device, const Scope<VulkanPhysicalDevice> &physicalDevice, uint32_t frameCount);
};

}
