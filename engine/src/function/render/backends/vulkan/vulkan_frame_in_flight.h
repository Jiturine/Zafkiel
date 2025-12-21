#pragma once
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>
#include "vulkan_command_manager.h"
#include "vulkan_device.h"

namespace Zafkiel 
{
class VulkanContext;
class VulkanFrameInFlight final
{
  public:
    vk::raii::CommandBuffer cmdBuf;
    vk::raii::Fence fence;
    vk::raii::Semaphore imageAvailableSem;
    vk::raii::Semaphore renderFinishedSem;
    const Scope<VulkanDevice> &device;
    VulkanFrameInFlight(const Scope<VulkanCommandManager> &commandManager, const Scope<VulkanDevice> &device);
};
}