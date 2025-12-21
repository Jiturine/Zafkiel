#pragma once
#include <vulkan/vulkan.hpp>
#include "vulkan_device.h"

namespace Zafkiel 
{
class VulkanCommandManager final
{
  public:
    VulkanCommandManager(const Scope<VulkanDevice> &device, const Scope<VulkanPhysicalDevice> &physicalDevice);

    vk::raii::CommandPool &GetCommandPool() { return commandPool; }

    std::vector<vk::raii::CommandBuffer> CreateCommandBuffers(uint32_t count) const;
    vk::raii::CommandBuffer CreateOneCommandBuffer() const;

  private:
    vk::raii::CommandPool commandPool;
    const Scope<VulkanDevice>& device;
};
}