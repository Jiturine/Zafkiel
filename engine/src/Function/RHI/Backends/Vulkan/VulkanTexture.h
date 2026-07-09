#pragma once
#include "Function/RHI/RHIResources.h"
#include <vulkan/vulkan_raii.hpp>

namespace Zafkiel
{

class VulkanDevice;

vk::Format ImageFormatToVulkanType(ImageFormat format);

ImageFormat VulkanFormatToImageFormat(vk::Format format);

vk::ImageUsageFlags ImageUsageFlagsToVulkanType(ImageUsageFlags usage);

vk::ImageLayout ImageLayoutToVulkanType(ImageLayout layout);

vk::SampleCountFlagBits SampleCountToVulkanType(uint32 sampleCount);

vk::ImageAspectFlags ImageFormatToVulkanImageAspect(ImageFormat format);

vk::SamplerAddressMode TextureWrapToVulkanAddressMode(TextureWrap wrap);

vk::Filter FilterTypeToVulkanType(TextureFilter filter);

class VulkanTextureBase : public RHITexture
{
  public:
    VulkanTextureBase(const RHITextureDesc &desc)
        : RHITexture(desc) {}

    virtual const vk::Image &GetLowLevelImage() = 0;

    virtual vk::raii::ImageView &GetImageView() = 0;
};

class VulkanTexture : public VulkanTextureBase
{
  public:
    VulkanTexture(const RHITextureDesc &desc, RHICommandList &cmdList, VulkanDevice &device, Buffer data = nullptr);

    ~VulkanTexture();

    vk::raii::Image &GetImage() { return image; }

    virtual const vk::Image &GetLowLevelImage() override { return *image; }

    virtual vk::raii::ImageView &GetImageView() override { return imageView; }
  
    vk::raii::Sampler &GetSampler() { return sampler; }

  private:
    VulkanDevice &device;

    vk::raii::Image image;
    vk::raii::ImageView imageView;
    vk::raii::Sampler sampler;
    vk::raii::DeviceMemory memory;
};
}
