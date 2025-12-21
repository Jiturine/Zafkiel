#pragma once
#include <vulkan/vulkan_raii.hpp>

namespace Zafkiel 
{

struct VulkanBufferSpecification 
{
    uint32_t size;
    vk::BufferUsageFlags usage;
    vk::MemoryPropertyFlags property;
};

class VulkanDevice;
class VulkanPhysicalDevice;
class VulkanBuffer final
{
  public:
    vk::raii::Buffer buffer;
    vk::raii::DeviceMemory memory;
    size_t size;
    size_t memorySize;
    VulkanBuffer(const VulkanBufferSpecification& spec, const Scope<VulkanDevice> &device, const Scope<VulkanPhysicalDevice> &physicalDevice);
};
}