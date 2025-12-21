#include "vulkan_index_buffer.h"
#include "vulkan_context.h"

namespace Zafkiel 
{
VulkanIndexBufferBackend::VulkanIndexBufferBackend(const uint32_t* indices, uint32_t count, const Scope<VulkanDevice> &device, const Scope<VulkanPhysicalDevice> &physicalDevice, const Scope<VulkanCommandManager> &commandManager)
{
    uint32_t size = (uint32_t)sizeof(uint32_t) * count;
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
        .usage = vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst,
        .property = vk::MemoryPropertyFlagBits::eDeviceLocal
    };
    buffer = CreateScope<VulkanBuffer>(deviceBufferSpec, device, physicalDevice);

    void *ptr = stagingBuffer->memory.mapMemory(0, stagingBuffer->size);
    memcpy(ptr, indices, size);
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

Scope<IndexBuffer> VulkanIndexBufferFactory::Create(const uint32_t* indices, uint32_t count, const Scope<VulkanDevice> &device, const Scope<VulkanPhysicalDevice> &physicalDevice, const Scope<VulkanCommandManager> &commandManager)
{
    auto backend = CreateScope<VulkanIndexBufferBackend>(indices, count, device, physicalDevice, commandManager);
    return CreateScope<IndexBuffer>(indices, count, std::move(backend));
}

}