#pragma once
#include "vulkan_device.h"

namespace Zafkiel 
{
class VulkanDescriptorManager final
{
  public:
    VulkanDescriptorManager(const Scope<VulkanDevice> &device);

    vk::raii::DescriptorPool &GetDescriptorPool() { return descriptorPool; }

  private:
    vk::raii::DescriptorPool descriptorPool;
};
}