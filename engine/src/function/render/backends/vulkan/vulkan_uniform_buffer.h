#pragma once
#include "function/render/uniform_buffer.h"
#include "vulkan_buffer.h"

namespace Zafkiel 
{

class VulkanUniformBufferBackend : public UniformBufferBackend
{
  public:
    VulkanUniformBufferBackend(uint32_t size, const Scope<VulkanDevice> &device, const Scope<VulkanPhysicalDevice> &physicalDevice);
    ~VulkanUniformBufferBackend();
    vk::raii::Buffer &GetHandle() { return buffer->buffer; }
    const vk::raii::Buffer &GetHandle() const { return buffer->buffer; }

    void SetData(uint32_t offset, uint32_t size, const void *data) const; 
    
  private:
    Scope<VulkanBuffer> buffer;
    std::byte *mappedMemory;
};

class VulkanUniformBufferFactory
{
  public:
    static Scope<UniformBuffer> Create(uint32_t size, const Scope<VulkanDevice> &device, const Scope<VulkanPhysicalDevice> &physicalDevice);
};

}