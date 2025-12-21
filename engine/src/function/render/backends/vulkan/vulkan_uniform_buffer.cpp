#include "vulkan_uniform_buffer.h"

namespace Zafkiel
{

VulkanUniformBufferBackend::VulkanUniformBufferBackend(uint32_t size, const Scope<VulkanDevice> &device, const Scope<VulkanPhysicalDevice> &physicalDevice)
{
    VulkanBufferSpecification bufferSpec
    {
        .size = size,
        .usage = vk::BufferUsageFlagBits::eUniformBuffer,
        .property = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
    };
    buffer = CreateScope<VulkanBuffer>(bufferSpec, device, physicalDevice);
    
    mappedMemory = reinterpret_cast<std::byte*>(buffer->memory.mapMemory(0, buffer->size));
}

VulkanUniformBufferBackend::~VulkanUniformBufferBackend()
{
    buffer->memory.unmapMemory();
}

void VulkanUniformBufferBackend::SetData(uint32_t offset, uint32_t size, const void *data) const
{
    memcpy(mappedMemory + offset, data, size);
}

Scope<UniformBuffer> VulkanUniformBufferFactory::Create(uint32_t size, const Scope<VulkanDevice> &device, const Scope<VulkanPhysicalDevice> &physicalDevice)
{
    auto backend = CreateScope<VulkanUniformBufferBackend>(size, device, physicalDevice);
    return CreateScope<UniformBuffer>(size, std::move(backend));
}

}