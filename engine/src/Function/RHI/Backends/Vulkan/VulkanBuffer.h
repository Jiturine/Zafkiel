#pragma once
#include <vulkan/vulkan_raii.hpp>
#include "Function/RHI/RHIResources.h"

namespace Zafkiel 
{

class VulkanDevice;

struct InternalVulkanBufferDesc
{
    uint32 size;
    vk::BufferUsageFlags usage;
    vk::MemoryPropertyFlags property;
};

class InternalVulkanBuffer
{
  public:
    vk::raii::Buffer buffer;
    vk::raii::DeviceMemory memory;
    size_t size;
    size_t memorySize;
    InternalVulkanBuffer(const InternalVulkanBufferDesc& desc, VulkanDevice &device);
};

class VulkanBuffer : public RHIBuffer 
{
  public:
    VulkanBuffer(const RHIBufferDesc &desc, RHICommandList &cmdList, VulkanDevice &device, const void *data = nullptr);

    void Write(RHICommandList &cmdList, Buffer data);
  
    uint32 GetSize() { return buffer->size; }
  
    vk::raii::Buffer &GetBuffer() { return buffer->buffer; }

  private:
    VulkanDevice &device;
    Scope<InternalVulkanBuffer> buffer;
};
}
