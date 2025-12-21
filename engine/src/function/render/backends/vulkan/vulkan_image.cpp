#include "vulkan_image.h"

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
    case DEPTH24STENCIL8: return vk::Format::eD24UnormS8Uint;
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
    case eD24UnormS8Uint: return ImageFormat::DEPTH24STENCIL8;
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
    case DepthAttachment: return vk::ImageLayout::eDepthAttachmentOptimal;
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

static vk::ImageAspectFlags ImageFormatToVulkanImageAspect(ImageFormat format)
{
    switch (format) 
    {
        using enum ImageFormat;
    case DEPTH24STENCIL8: return vk::ImageAspectFlagBits::eDepth | vk::ImageAspectFlagBits::eStencil;
    default: return vk::ImageAspectFlagBits::eColor; 
    }
}

VulkanImageBackend::VulkanImageBackend(const ImageSpecification &spec, const Scope<VulkanDevice> &device, const Scope<VulkanPhysicalDevice> &physicalDevice, uint32_t frameCount)
    : device(device), physicalDevice(physicalDevice), imageFormat(spec.format)
{
    for (auto imageUsage : spec.usages)
    {
        usages |= ImageUsageToVulkanType(imageUsage);
    }
    imageCount = 1;
    if (spec.updateFrequency == ImageUpdateFrequency::Transient)
    {
        imageCount = frameCount;
    }
    format = ImageFormatToVulkanType(spec.format);
    sampleCount = SampleCountToVulkanType(spec.samples);

    Invalidate(spec.width, spec.height);
}

void VulkanImageBackend::Resize(uint32_t width, uint32_t height)
{
    Invalidate(width, height);
}

void VulkanImageBackend::Invalidate(uint32_t width, uint32_t height)
{
    device->GetHandle().waitIdle();
    memories.clear();
    imageViews.clear();
    images.clear();
    // 创建 vk::Image
    vk::ImageCreateInfo imageCreateInfo;
    imageCreateInfo.setImageType(vk::ImageType::e2D)
                   .setArrayLayers(1)
                   .setMipLevels(1)
                   .setExtent({width, height, 1})
                   .setFormat(format)
                   .setTiling(vk::ImageTiling::eOptimal)
                   .setInitialLayout(vk::ImageLayout::eUndefined)
                   .setUsage(usages)
                   .setSamples(sampleCount);
    for (size_t i = 0; i < imageCount; i++)
    {
        images.emplace_back(device->GetHandle().createImage(imageCreateInfo));
    }

    // 查询分配内存信息
    auto requirements = images[0].getMemoryRequirements();

    uint32_t index = 0;
    auto properties = physicalDevice->GetHandle().getMemoryProperties();
    for (size_t i = 0; i < properties.memoryTypeCount; i++)
    {
        if ((1 << i) & requirements.memoryTypeBits && properties.memoryTypes[i].propertyFlags & vk::MemoryPropertyFlagBits::eDeviceLocal)
        {
            index = i;
            break;
        }
    }

    // 分配内存
    vk::MemoryAllocateInfo allocInfo;
    allocInfo.setMemoryTypeIndex(index)
             .setAllocationSize(requirements.size);
    for (size_t i = 0; i < imageCount; i++)
    {
        memories.emplace_back(device->GetHandle().allocateMemory(allocInfo));
    }

    // 绑定内存
    for (size_t i = 0; i < imageCount; i++)
    {
        images[i].bindMemory(memories[i],  0);
    }

    // TODO: 转换内存布局

    // 创建 ImageView

    for (size_t i = 0; i < imageCount; i++)
    {
        vk::ImageViewCreateInfo imageViewCreateInfo;
        vk::ComponentMapping mapping;
        vk::ImageSubresourceRange range;
        range.setBaseMipLevel(0)
             .setLevelCount(1)
             .setBaseArrayLayer(0)
             .setLayerCount(1)
             .setAspectMask(ImageFormatToVulkanImageAspect(imageFormat));
        imageViewCreateInfo.setImage(images[i])
                           .setViewType(vk::ImageViewType::e2D)
                           .setComponents(mapping)
                           .setFormat(format)
                           .setSubresourceRange(range);
        imageViews.emplace_back(device->GetHandle().createImageView(imageViewCreateInfo));
    }
}


Scope<Image> VulkanImageFactory::Create(const ImageSpecification &spec, const Scope<VulkanDevice> &device, const Scope<VulkanPhysicalDevice> &physicalDevice, uint32_t frameCount)
{
    auto backend = CreateScope<VulkanImageBackend>(spec, device, physicalDevice, frameCount);
    return CreateScope<Image>(spec, std::move(backend));
}

}
