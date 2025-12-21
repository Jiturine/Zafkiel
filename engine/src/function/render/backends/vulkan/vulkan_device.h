#pragma once
#include <vulkan/vulkan_raii.hpp>

namespace Zafkiel 
{

class VulkanPhysicalDevice final
{
public:
    VulkanPhysicalDevice(const vk::raii::PhysicalDevice &device, const vk::raii::SurfaceKHR &surface);

    friend class VulkanDevice;
    friend class VulkanSwapchain;

    vk::raii::PhysicalDevice &GetHandle() { return physicalDevice; }
    const vk::raii::PhysicalDevice &GetHandle() const { return physicalDevice; }
    uint32_t GetGraphicsQueueIndex() const { return queueFamilyIndices.graphicsQueueIndex.value(); }
    uint32_t GetPresentQueueIndex() const { return queueFamilyIndices.presentQueueIndex.value(); }
    uint32_t GetMinUniformBufferOffsetAlignment() const { return minUniformBufferOffsetAlignment; }

  private:
    struct QueueFamilyIndices
    {
        std::optional<uint32_t> graphicsQueueIndex;
        std::optional<uint32_t> presentQueueIndex;
        bool IsComplete() const 
        { 
            return graphicsQueueIndex.has_value() && presentQueueIndex.has_value();
        }
    } queueFamilyIndices;
    uint32_t minUniformBufferOffsetAlignment;

    vk::raii::PhysicalDevice physicalDevice;
};

class VulkanDevice
{
  public:
    VulkanDevice(const Scope<VulkanPhysicalDevice>& physicalDevice);
    vk::raii::Device &GetHandle() { return device; }
    const vk::raii::Device &GetHandle() const { return device; }
    vk::raii::Queue &GetGraphicsQueue() { return graphicsQueue; }
    vk::raii::Queue &GetPresentQueue() { return presentQueue; }

    friend class VulkanSwapchain;

  private:
    vk::raii::Device device;
    vk::raii::Queue graphicsQueue;
    vk::raii::Queue presentQueue;
};
}