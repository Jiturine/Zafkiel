#include "vulkan_texture.h"
#include "vulkan_buffer.h"
#include "vulkan/vulkan.hpp"
#include "vulkan_image.h"

namespace Zafkiel
{
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

VulkanTexture2DBackend::VulkanTexture2DBackend(const Texture2DSpecification &spec, const Scope<VulkanDevice> &device, const Scope<VulkanPhysicalDevice> &physicalDevice, const Scope<VulkanCommandManager> &commandManager, uint32_t frameCount)
    : sampler(nullptr), device(device), physicalDevice(physicalDevice), commandManager(commandManager), frameCount(frameCount)
{
    vk::SamplerCreateInfo createInfo;
    createInfo.setMagFilter(FilterTypeToVulkanType(spec.filter))
        .setMinFilter(FilterTypeToVulkanType(spec.filter))
        .setAddressModeU(TextureWrapToVulkanAddressMode(spec.wrap))
        .setAddressModeV(TextureWrapToVulkanAddressMode(spec.wrap))
        .setAddressModeW(TextureWrapToVulkanAddressMode(spec.wrap))
        .setAnisotropyEnable(false)
        .setBorderColor(vk::BorderColor::eFloatOpaqueBlack)
        .setUnnormalizedCoordinates(false)
        .setCompareEnable(false)
        .setMipmapMode(vk::SamplerMipmapMode::eLinear);
    
    sampler = device->GetHandle().createSampler(createInfo);
}
void VulkanTexture2DBackend::SetData(Observer<Image> image, Buffer buffer)
{
    // 转换到 TransferDst Layout
    ExecuteCmd([&](vk::raii::CommandBuffer &cmdBuf){
        vk::ImageMemoryBarrier barrier;
        vk::ImageSubresourceRange range;
        range.setLayerCount(1)
             .setBaseArrayLayer(0)
             .setLevelCount(1)
             .setBaseMipLevel(0)
             .setAspectMask(vk::ImageAspectFlagBits::eColor);
        barrier.setImage(image->GetBackend().As<VulkanImageBackend>()->GetImage(0))
               .setOldLayout(vk::ImageLayout::eUndefined)
               .setNewLayout(vk::ImageLayout::eTransferDstOptimal)
               .setDstQueueFamilyIndex(vk::QueueFamilyIgnored)
               .setSrcQueueFamilyIndex(vk::QueueFamilyIgnored)
               .setDstAccessMask(vk::AccessFlagBits::eTransferWrite)
               .setSubresourceRange(range);
        cmdBuf.pipelineBarrier(vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer, {}, {}, nullptr, barrier);
    });

    // 传输数据
    VulkanBufferSpecification bufferSpec 
    {
        .size = buffer.Size<uint8_t>(),
        .usage = vk::BufferUsageFlagBits::eTransferSrc,
        .property = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
    };
    auto vkBuffer = CreateScope<VulkanBuffer>(bufferSpec, device, physicalDevice);
    void *map = vkBuffer->memory.mapMemory(0, buffer.Size<uint8_t>());
    memcpy(map, buffer.Data<uint8_t>(), buffer.Size<uint8_t>());
    vkBuffer->memory.unmapMemory();

    ExecuteCmd([&](vk::raii::CommandBuffer &cmdBuf){
        vk::BufferImageCopy region;
        vk::ImageSubresourceLayers subsource;
        subsource.setAspectMask(vk::ImageAspectFlagBits::eColor)
            .setBaseArrayLayer(0)
            .setMipLevel(0)
            .setLayerCount(1);
        region.setBufferImageHeight(0)
            .setBufferOffset(0)
            .setImageOffset(0)
            .setImageExtent({image->GetWidth(), image->GetHeight(), 1})
            .setBufferRowLength(0)
            .setImageSubresource(subsource);
        cmdBuf.copyBufferToImage(vkBuffer->buffer, image->GetBackend().As<VulkanImageBackend>()->GetImage(0), vk::ImageLayout::eTransferDstOptimal, region);
    });

    // 转换到Shader ReadOnly Layout
    ExecuteCmd([&](vk::raii::CommandBuffer &cmdBuf){
        vk::ImageMemoryBarrier barrier;
        vk::ImageSubresourceRange range;
        range.setLayerCount(1)
            .setBaseArrayLayer(0)
            .setLevelCount(1)
            .setBaseMipLevel(0)
            .setAspectMask(vk::ImageAspectFlagBits::eColor);
        barrier.setImage(image->GetBackend().As<VulkanImageBackend>()->GetImage(0))
            .setOldLayout(vk::ImageLayout::eTransferDstOptimal)
            .setNewLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
            .setDstQueueFamilyIndex(vk::QueueFamilyIgnored)
            .setSrcQueueFamilyIndex(vk::QueueFamilyIgnored)
            .setSrcAccessMask(vk::AccessFlagBits::eTransferWrite)
            .setDstAccessMask(vk::AccessFlagBits::eShaderRead)
            .setSubresourceRange(range);
        cmdBuf.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, {}, {}, nullptr, barrier);
    });
}

void VulkanTexture2DBackend::ExecuteCmd(std::function<void(vk::raii::CommandBuffer&)> func)
{
    auto cmdBuf = commandManager->CreateOneCommandBuffer();
    vk::CommandBufferBeginInfo begin;
    begin.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    cmdBuf.begin(begin);
    {
        func(cmdBuf);
    }
    cmdBuf.end();

    vk::SubmitInfo submit;
    submit.setCommandBuffers(*cmdBuf);
    device->GetGraphicsQueue().submit(submit);
    device->GetGraphicsQueue().waitIdle();
}

Scope<Texture2D> VulkanTexture2DFactory::Create(const Texture2DSpecification &spec, const Scope<VulkanDevice> &device, const Scope<VulkanPhysicalDevice> &physicalDevice, const Scope<VulkanCommandManager> &commandManager, uint32_t frameCount)
{
    auto backend = CreateScope<VulkanTexture2DBackend>(spec, device, physicalDevice, commandManager, frameCount);

    ImageSpecification imageSpec
    {
        .format = spec.format,
        .usages = spec.usages,
        .updateFrequency = spec.updateFrequency,
        .width = spec.width,
        .height = spec.height,
        .samples = spec.samples
    };
    auto texture2D = CreateScope<Texture2D>(spec, std::move(backend));
    AccessImage(texture2D) = VulkanImageFactory::Create(imageSpec, device, physicalDevice, frameCount);

    return texture2D;
}
Scope<Texture2D> VulkanTexture2DFactory::Create(const Texture2DSpecification &spec, Buffer buffer, const Scope<VulkanDevice> &device, const Scope<VulkanPhysicalDevice> &physicalDevice, const Scope<VulkanCommandManager> &commandManager, uint32_t frameCount)
{
    auto backend = CreateScope<VulkanTexture2DBackend>(spec, device, physicalDevice, commandManager, frameCount);
    ImageSpecification imageSpec
    {
        .format = spec.format,
        .usages = spec.usages,
        .updateFrequency = spec.updateFrequency,
        .width = spec.width,
        .height = spec.height,
        .samples = spec.samples
    };
    auto texture2D = CreateScope<Texture2D>(spec, std::move(backend));
    if (std::find(imageSpec.usages.begin(), imageSpec.usages.end(), ImageUsage::Upload) == imageSpec.usages.end())
    {
        imageSpec.usages.push_back(ImageUsage::Upload);
    }
    AccessImage(texture2D) = VulkanImageFactory::Create(imageSpec, device, physicalDevice, frameCount);

    texture2D->GetBackend().As<VulkanTexture2DBackend>()->SetData(texture2D->GetImage(), buffer);
    return texture2D;
}

}