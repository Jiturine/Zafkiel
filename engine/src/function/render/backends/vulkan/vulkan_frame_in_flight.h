#pragma once
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>
#include "function/render/backends/vulkan/vulkan_command_manager.h"
#include "function/render/backends/vulkan/vulkan_device.h"

namespace Zafkiel 
{
class VulkanFrameInFlight final
{
  public:
    vk::raii::CommandBuffer cmdBuf;
    vk::raii::Fence fence;
    vk::raii::Semaphore imageAvailableSem;
    vk::raii::Semaphore renderFinishedSem;
    Borrow<VulkanDevice> device;
    VulkanFrameInFlight(Borrow<VulkanCommandManager> commandManager, Borrow<VulkanDevice> device);
};
}