#pragma once
#include "function/render/backends/vulkan/vulkan_device.h"

namespace Zafkiel 
{
class VulkanCommandManager final
{
  public:
    VulkanCommandManager(Borrow<VulkanDevice> device, Borrow<VulkanPhysicalDevice> physicalDevice);

    vk::raii::CommandPool &GetCommandPool() { return commandPool; }

    std::vector<vk::raii::CommandBuffer> CreateCommandBuffers(uint32_t count) const;
    vk::raii::CommandBuffer CreateOneCommandBuffer() const;

  private:
    vk::raii::CommandPool commandPool;
    Borrow<VulkanDevice> device;
};
}