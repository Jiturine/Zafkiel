#pragma once
#include "function/render/backends/vulkan/vulkan_device.h"

namespace Zafkiel 
{
class VulkanDescriptorManager final
{
  public:
    VulkanDescriptorManager(Borrow<VulkanDevice> device);

    vk::raii::DescriptorPool &GetDescriptorPool() { return descriptorPool; }
    const vk::raii::DescriptorPool &GetDescriptorPool() const { return descriptorPool; }

  private:
    vk::raii::DescriptorPool descriptorPool;
};
}