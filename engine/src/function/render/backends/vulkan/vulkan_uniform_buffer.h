#pragma once
#include "function/render/uniform_buffer.h"
#include "function/render/backends/vulkan/vulkan_buffer.h"

namespace Zafkiel 
{

class VulkanUniformBufferBackend final : public UniformBufferBackend
{
  public:
    VulkanUniformBufferBackend(Scope<VulkanBuffer> buffer);
    
    ~VulkanUniformBufferBackend();
  
    vk::raii::Buffer &GetHandle() { return buffer->buffer; }
    const vk::raii::Buffer &GetHandle() const { return buffer->buffer; }

    void SetData(uint32_t offset, uint32_t size, const void *data) const; 
    
  private:
    Scope<VulkanBuffer> buffer;
    std::byte *mappedMemory;
};

}