#include "function/render/backends/vulkan/vulkan_uniform_buffer.h"

namespace Zafkiel
{

VulkanUniformBufferBackend::VulkanUniformBufferBackend(Scope<VulkanBuffer> buffer)
    : buffer(std::move(buffer))
{
    mappedMemory = reinterpret_cast<std::byte*>(this->buffer->memory.mapMemory(0, this->buffer->size));
}

VulkanUniformBufferBackend::~VulkanUniformBufferBackend()
{
    buffer->memory.unmapMemory();
}

void VulkanUniformBufferBackend::SetData(uint32_t offset, uint32_t size, const void *data) const
{
    memcpy(mappedMemory + offset, data, size);
}


}