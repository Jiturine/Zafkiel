#include "function/render/backends/vulkan/vulkan_image.h"

namespace Zafkiel
{

vk::Format ImageFormatToVulkanType(ImageFormat format)
{
    switch (format)
    {
        using enum ImageFormat;
    case R8: return vk::Format::eR8Unorm;
    case RG8: return vk::Format::eR8G8Unorm;
    case RGB8: return vk::Format::eR8G8B8Unorm;
    case RGBA8: return vk::Format::eR8G8B8A8Unorm;
    case BGR8: return vk::Format::eB8G8R8Unorm;
    case BGRA8: return vk::Format::eB8G8R8A8Unorm;
    case R8_sRGB: return vk::Format::eR8Srgb;
    case RG8_sRGB: return vk::Format::eR8G8Srgb;
    case RGB8_sRGB: return vk::Format::eR8G8B8A8Srgb;
    case RGBA8_sRGB: return vk::Format::eR8G8B8A8Srgb;
    case BGR8_sRGB: return vk::Format::eB8G8R8Srgb;
    case BGRA8_sRGB: return vk::Format::eB8G8R8A8Srgb;
    case RGB16F: return vk::Format::eR16G16B16Sfloat;
    case RGBA16F: return vk::Format::eR16G16B16A16Sfloat;
    case RGBA32F: return vk::Format::eR32G32B32A32Sfloat;
    case R32UI: return vk::Format::eR32Uint;
    case R32F: return vk::Format::eR32Sfloat;
    case DEPTH24STENCIL8: return vk::Format::eD24UnormS8Uint;
    case DEPTH32F: return vk::Format::eD32Sfloat;
    default:
        Log::Error("Unsupported Format!");
        return vk::Format::eUndefined;
    }
}

ImageFormat VulkanFormatToImageFormat(vk::Format format)
{
    switch (format)
    {
        using enum vk::Format;
    case eR8Unorm: return ImageFormat::R8;
    case eR8G8Unorm: return ImageFormat::RG8;
    case eR8G8B8Unorm: return ImageFormat::RGB8;
    case eR8G8B8A8Unorm: return ImageFormat::RGBA8;
    case eB8G8R8Unorm: return ImageFormat::BGR8;
    case eB8G8R8A8Unorm: return ImageFormat::BGRA8;
    case eR8Srgb: return ImageFormat::R8_sRGB;
    case eR8G8Srgb: return ImageFormat::RG8_sRGB;
    case eR8G8B8Srgb: return ImageFormat::RGB8_sRGB;
    case eR8G8B8A8Srgb: return ImageFormat::RGBA8_sRGB;
    case eB8G8R8Srgb: return ImageFormat::BGR8_sRGB;
    case eB8G8R8A8Srgb: return ImageFormat::BGRA8_sRGB;
    case eR16G16B16Sfloat: return ImageFormat::RGB16F;
    case eR16G16B16A16Sfloat: return ImageFormat::RGBA16F;
    case eR32G32B32A32Sfloat: return ImageFormat::RGBA32F;
    case eR32Uint: return ImageFormat::R32UI;
    case eR32Sfloat: return ImageFormat::R32F;
    case eD24UnormS8Uint: return ImageFormat::DEPTH24STENCIL8;
    case eD32Sfloat: return ImageFormat::DEPTH32F;
    default:
        Log::Error("Unsupported Format!");
        return ImageFormat::None;
    }
}

vk::ImageUsageFlagBits ImageUsageToVulkanType(ImageUsage usage)
{
    switch (usage)
    {
        using enum ImageUsage;
    case Upload: return vk::ImageUsageFlagBits::eTransferDst;
    case Sampled: return vk::ImageUsageFlagBits::eSampled;
    case ColorAttachment: return vk::ImageUsageFlagBits::eColorAttachment;
    case DepthAttachment: return vk::ImageUsageFlagBits::eDepthStencilAttachment;
    default:
        Log::Error("Unsupported Usage!");
        return vk::ImageUsageFlagBits::eSampled;
    }
}

vk::ImageLayout ImageLayoutToVulkanType(ImageLayout layout)
{
    switch (layout)
    {
        using enum ImageLayout;
    case Undefined: return vk::ImageLayout::eUndefined;
    case ShaderReadOnly: return vk::ImageLayout::eShaderReadOnlyOptimal;
    case ColorAttachment: return vk::ImageLayout::eColorAttachmentOptimal;
    case DepthAttachment: return vk::ImageLayout::eDepthStencilAttachmentOptimal;
    case DepthStencilAttachment: return vk::ImageLayout::eDepthStencilAttachmentOptimal;
    case PresentSrc: return vk::ImageLayout::ePresentSrcKHR;
    default:
        Log::Error("Unsupported Layout");
        return vk::ImageLayout::eUndefined;
    }
}

vk::SampleCountFlagBits SampleCountToVulkanType(uint32_t sampleCount)
{
    switch (sampleCount) 
    {
    case 1: return vk::SampleCountFlagBits::e1;
    case 2: return vk::SampleCountFlagBits::e2;
    case 4: return vk::SampleCountFlagBits::e4;
    case 8: return vk::SampleCountFlagBits::e8;
    case 16: return vk::SampleCountFlagBits::e16;
    case 32: return vk::SampleCountFlagBits::e32;
    case 64: return vk::SampleCountFlagBits::e64;
    default:
        Log::Error("Unsupported Sample Count: {} !", sampleCount);
        return vk::SampleCountFlagBits::e1;
    }
}

vk::ImageAspectFlags ImageFormatToVulkanImageAspect(ImageFormat format)
{
    switch (format) 
    {
        using enum ImageFormat;
    case DEPTH24STENCIL8: return vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
    case DEPTH32F: return vk::ImageAspectFlagBits::eDepth;
    default: return vk::ImageAspectFlagBits::eColor; 
    }
}

VulkanImageBackend::VulkanImageBackend(uint32_t imageCount, std::vector<vk::raii::Image> images, std::vector<vk::raii::ImageView> imageViews, std::vector<vk::raii::DeviceMemory> memories)
    : imageCount(imageCount), images(std::move(images)), imageViews(std::move(imageViews)), memories(std::move(memories))
{
}


}
