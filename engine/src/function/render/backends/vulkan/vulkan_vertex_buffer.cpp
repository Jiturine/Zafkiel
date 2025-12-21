#include "vulkan_vertex_buffer.h"

namespace Zafkiel
{

vk::Format ShaderDataTypeToVulkanFormat(ShaderFundamentalType type)
{
    switch (type)
    {
        using enum ShaderReflection::FundamentalKind;
    case Float2:
        return vk::Format::eR32G32Sfloat;
    case Float3:
        return vk::Format::eR32G32B32Sfloat;
    case Float4:
        return vk::Format::eR32G32B32A32Sfloat;
    default:
        Log::Error("Failed to Convert to Vulkan Format!");
        return vk::Format::eUndefined;
    }
}

VulkanVertexBufferBackend::VulkanVertexBufferBackend(const float *vertices, uint32_t size, const Scope<VulkanDevice> &device, const Scope<VulkanPhysicalDevice> &physicalDevice, const Scope<VulkanCommandManager> &commandManager)
{
    VulkanBufferSpecification stagingBufferSpec
    {
        .size = size,
        .usage = vk::BufferUsageFlagBits::eTransferSrc,
        .property = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
    };
    auto stagingBuffer = CreateScope<VulkanBuffer>(stagingBufferSpec, device, physicalDevice);
    VulkanBufferSpecification deviceBufferSpec
    {
        .size = size,
        .usage = vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
        .property = vk::MemoryPropertyFlagBits::eDeviceLocal
    };
    buffer = CreateScope<VulkanBuffer>(deviceBufferSpec, device, physicalDevice);

    void *ptr = stagingBuffer->memory.mapMemory(0, stagingBuffer->size);
    memcpy(ptr, vertices, size);
    stagingBuffer->memory.unmapMemory();

    // 共享内存传输到GPU DeviceLocal
    auto cmdBuf = commandManager->CreateOneCommandBuffer();
    vk::CommandBufferBeginInfo begin;
    begin.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    cmdBuf.begin(begin);
    {
        vk::BufferCopy region;
        region.setSize(size)
              .setSrcOffset(0)
              .setDstOffset(0);
        cmdBuf.copyBuffer(stagingBuffer->buffer, buffer->buffer, region);
    }
    cmdBuf.end();

    vk::SubmitInfo submit;
    submit.setCommandBuffers(*cmdBuf);
    device->GetGraphicsQueue().submit(submit);

    device->GetHandle().waitIdle();
}

Scope<VertexBuffer> VulkanVertexBufferFactory::Create(const float *vertices, uint32_t size, const Scope<VulkanDevice> &device, const Scope<VulkanPhysicalDevice> &physicalDevice, const Scope<VulkanCommandManager> &commandManager)
{
    auto backend = CreateScope<VulkanVertexBufferBackend>(vertices, size, device, physicalDevice, commandManager);
    return CreateScope<VertexBuffer>(vertices, size, std::move(backend));  
}

}