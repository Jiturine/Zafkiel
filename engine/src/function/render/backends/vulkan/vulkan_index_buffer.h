#pragma once
#include "vulkan_buffer.h"
#include "function/render/index_buffer.h"
#include "vulkan_context.h"

namespace Zafkiel 
{
class VulkanIndexBufferBackend final : public IndexBufferBackend
{
  public:
    VulkanIndexBufferBackend(const uint32_t* indices, uint32_t count, const Scope<VulkanDevice> &device, const Scope<VulkanPhysicalDevice> &physicalDevice, const Scope<VulkanCommandManager> &commandManager);
    vk::raii::Buffer &GetHandle() { return buffer->buffer; }
    const vk::raii::Buffer &GetHandle() const { return buffer->buffer; }
  private:
    Scope<VulkanBuffer> buffer;
};

class VulkanIndexBufferFactory
{
  public:
    static Scope<IndexBuffer> Create(const uint32_t* indices, uint32_t count, const Scope<VulkanDevice> &device, const Scope<VulkanPhysicalDevice> &physicalDevice, const Scope<VulkanCommandManager> &commandManager);
};

}