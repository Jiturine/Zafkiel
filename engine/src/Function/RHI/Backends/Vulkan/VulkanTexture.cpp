#include "Function/RHI/Backends/Vulkan/VulkanTexture.h"
#include "Function/RHI/Backends/Vulkan/VulkanDevice.h"
#include "Function/RHI/Backends/Vulkan/VulkanBuffer.h"

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

vk::ImageUsageFlags ImageUsageFlagsToVulkanType(ImageUsageFlags usage)
{
    vk::ImageUsageFlags result;
    if (EnumHasAnyFlags(usage, ImageUsageFlags::Upload))
        result |= vk::ImageUsageFlagBits::eTransferDst;
    if (EnumHasAnyFlags(usage, ImageUsageFlags::Sampled))
        result |= vk::ImageUsageFlagBits::eSampled;
    if (EnumHasAnyFlags(usage, ImageUsageFlags::ColorAttachment))
        result |= vk::ImageUsageFlagBits::eColorAttachment;
    if (EnumHasAnyFlags(usage, ImageUsageFlags::DepthAttachment))
        result |= vk::ImageUsageFlagBits::eDepthStencilAttachment;
    return result;
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

vk::SampleCountFlagBits SampleCountToVulkanType(uint32 sampleCount)
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

vk::SamplerAddressMode TextureWrapToVulkanAddressMode(TextureWrap wrap)
{
    switch (wrap)
    {
        using enum TextureWrap;
    case Repeat: return vk::SamplerAddressMode::eRepeat;
    case Clamp: return vk::SamplerAddressMode::eClampToBorder;
    default:
        Log::Error("Unsupported TextureWrap!");
        return vk::SamplerAddressMode::eRepeat;
    }
}

vk::Filter FilterTypeToVulkanType(TextureFilter filter)
{
    switch (filter) 
    {
        using enum TextureFilter;
    case Nearest: return vk::Filter::eNearest;
    case Linear: return vk::Filter::eLinear;
    default:
        Log::Error("Unsupported TextureFilter!");
        return vk::Filter::eNearest;
    }
}

VulkanTexture::VulkanTexture(const RHITextureDesc &desc, RHICommandList &cmdList, VulkanDevice &device, Buffer data)
    : VulkanTextureBase(desc), image(nullptr), imageView(nullptr), sampler(nullptr), memory(nullptr)
{
    uint32 imageCount = 1;
    vk::ImageUsageFlags usages = ImageUsageFlagsToVulkanType(desc.usages);

    // 创建 vk::Image
    vk::ImageCreateInfo imageCreateInfo;
    imageCreateInfo.setImageType(vk::ImageType::e2D)
                   .setArrayLayers(1)
                   .setMipLevels(1)
                   .setExtent({desc.width, desc.height, 1})
                   .setFormat(ImageFormatToVulkanType(desc.format))
                   .setTiling(vk::ImageTiling::eOptimal)
                   .setInitialLayout(vk::ImageLayout::eUndefined)
                   .setUsage(usages)
                   .setSamples(SampleCountToVulkanType(desc.sampleCount));

    image = device.GetHandle().createImage(imageCreateInfo);

    // 查询分配内存信息
    auto requirements = image.getMemoryRequirements();

    uint32 index = 0;
    auto properties = device.GetPhysicalHandle().getMemoryProperties();
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
    memory = device.GetHandle().allocateMemory(allocInfo);

    // 绑定内存
    image.bindMemory(memory,  0);

    // 创建 ImageView
    vk::ImageViewCreateInfo imageViewCreateInfo;
    vk::ComponentMapping mapping;
    vk::ImageSubresourceRange range;
    range.setBaseMipLevel(0)
            .setLevelCount(1)
            .setBaseArrayLayer(0)
            .setLayerCount(1)
            .setAspectMask(ImageFormatToVulkanImageAspect(desc.format));
    imageViewCreateInfo.setImage(image)
                        .setViewType(vk::ImageViewType::e2D)
                        .setComponents(mapping)
                        .setFormat(ImageFormatToVulkanType(desc.format))
                        .setSubresourceRange(range);
    imageView = device.GetHandle().createImageView(imageViewCreateInfo);

    vk::SamplerCreateInfo createInfo;
    createInfo.setMagFilter(FilterTypeToVulkanType(desc.filter))
        .setMinFilter(FilterTypeToVulkanType(desc.filter))
        .setAddressModeU(TextureWrapToVulkanAddressMode(desc.wrap))
        .setAddressModeV(TextureWrapToVulkanAddressMode(desc.wrap))
        .setAddressModeW(TextureWrapToVulkanAddressMode(desc.wrap))
        .setAnisotropyEnable(false)
        .setBorderColor(vk::BorderColor::eFloatOpaqueBlack)
        .setUnnormalizedCoordinates(false)
        .setCompareEnable(false)
        .setMipmapMode(vk::SamplerMipmapMode::eLinear);

    sampler = device.GetHandle().createSampler(createInfo);

    if (data)
    {
        auto gfxContext = static_cast<VulkanGraphicsContext *>(cmdList.GetGraphicsContext());
        
        gfxContext->ImageMemoryBarrier(this, vk::AccessFlagBits::eNone, vk::AccessFlagBits::eTransferWrite,
                                              vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal,
                                              vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer);

        // 传输数据
        InternalVulkanBufferDesc stagingBufferDesc
        {
            .size = data.Size<uint8>(),
            .usage = vk::BufferUsageFlagBits::eTransferSrc,
            .property = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
        };
        auto vkBuffer = CreateScope<InternalVulkanBuffer>(stagingBufferDesc, device);

        void *map = vkBuffer->memory.mapMemory(0, data.Size<uint8_t>());
        memcpy(map, data.Data<uint8_t>(), data.Size<uint8_t>());
        vkBuffer->memory.unmapMemory();

        auto cmdBuf = gfxContext->GetCommandBuffer();

        vk::BufferImageCopy region;
        vk::ImageSubresourceLayers subsource;
        subsource.setAspectMask(vk::ImageAspectFlagBits::eColor)
                 .setBaseArrayLayer(0)
                 .setMipLevel(0)
                 .setLayerCount(1);
        region.setBufferImageHeight(0)
              .setBufferOffset(0)
              .setImageOffset(0)
              .setImageExtent({desc.width, desc.height, 1})
              .setBufferRowLength(0)
              .setImageSubresource(subsource);

        cmdBuf->GetHandle().copyBufferToImage(vkBuffer->buffer, image, vk::ImageLayout::eTransferDstOptimal, region);

        // 转换image layout从TRANSFER_DST_OPTIMAL到SHADER_READ_ONLY_OPTIMAL
        gfxContext->ImageMemoryBarrier(this, vk::AccessFlagBits::eTransferWrite, vk::AccessFlagBits::eShaderRead,
                                              vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal,
                                              vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader);

        cmdList.SubmitAndWaitIdle();
    }
}

}
