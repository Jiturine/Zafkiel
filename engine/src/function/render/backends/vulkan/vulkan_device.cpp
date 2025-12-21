#include "vulkan_device.h"
#include "vulkan/vulkan.hpp"
#include <vulkan/vulkan.hpp>

namespace Zafkiel 
{

VulkanPhysicalDevice::VulkanPhysicalDevice(const vk::raii::PhysicalDevice &device, const vk::raii::SurfaceKHR &surface)
    : physicalDevice(device)
{
    // 选择队列族
    auto properties = device.getQueueFamilyProperties();
    for (int i = 0; i < properties.size(); i++)
    {
        if (properties[i].queueFlags & vk::QueueFlagBits::eGraphics)
            queueFamilyIndices.graphicsQueueIndex = i;
        if (physicalDevice.getSurfaceSupportKHR(i, surface))
            queueFamilyIndices.presentQueueIndex = i;
        if (queueFamilyIndices.IsComplete())
            break;
    }
    if (!queueFamilyIndices.IsComplete())
    {
        Log::Error("Queue Family Unavailable!");
    }
    minUniformBufferOffsetAlignment = physicalDevice.getProperties().limits.minUniformBufferOffsetAlignment;
}


VulkanDevice::VulkanDevice(const Scope<VulkanPhysicalDevice>& physicalDevice)
    : device(nullptr), graphicsQueue(nullptr), presentQueue(nullptr)
{
    vk::StructureChain<vk::DeviceCreateInfo, 
                       vk::PhysicalDeviceSwapchainMaintenance1FeaturesEXT, 
                       vk::PhysicalDeviceGraphicsPipelineLibraryFeaturesEXT> deviceChain;
    
    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    float priorities = 1.0f;

    if (physicalDevice->queueFamilyIndices.graphicsQueueIndex == physicalDevice->queueFamilyIndices.presentQueueIndex)
    {
        vk::DeviceQueueCreateInfo queueCreateInfo;
        queueCreateInfo.setQueuePriorities(priorities)
                       .setQueueCount(1)
                       .setQueueFamilyIndex(physicalDevice->queueFamilyIndices.graphicsQueueIndex.value());
        queueCreateInfos.push_back(queueCreateInfo);
    }
    else
    {
        vk::DeviceQueueCreateInfo queueCreateInfo;
        queueCreateInfo.setQueuePriorities(priorities)
                       .setQueueCount(1)
                       .setQueueFamilyIndex(physicalDevice->queueFamilyIndices.graphicsQueueIndex.value());
        queueCreateInfos.push_back(queueCreateInfo);
        queueCreateInfo.setPQueuePriorities(&priorities)
                       .setQueueCount(1)
                       .setQueueFamilyIndex(physicalDevice->queueFamilyIndices.presentQueueIndex.value());
        queueCreateInfos.push_back(queueCreateInfo);
    }
    deviceChain.get().setQueueCreateInfos(queueCreateInfos);

    std::vector<const char *> deviceExtensions 
    { 
        vk::KHRSwapchainExtensionName,               //
        vk::EXTSwapchainMaintenance1ExtensionName,   // swapchain 同步扩展 
        vk::EXTGraphicsPipelineLibraryExtensionName, // 
        vk::KHRPipelineLibraryExtensionName          // graphics_pipeline允许空set
    };
    deviceChain.get().setEnabledExtensionCount(deviceExtensions.size())
                     .setPpEnabledExtensionNames(deviceExtensions.data());
    deviceChain.get<vk::PhysicalDeviceSwapchainMaintenance1FeaturesEXT>().setSwapchainMaintenance1(true);
    deviceChain.get<vk::PhysicalDeviceGraphicsPipelineLibraryFeaturesEXT>().setGraphicsPipelineLibrary(true);

    device = physicalDevice->physicalDevice.createDevice(deviceChain.get());

    graphicsQueue = device.getQueue(physicalDevice->queueFamilyIndices.graphicsQueueIndex.value(), 0);
    presentQueue = device.getQueue(physicalDevice->queueFamilyIndices.presentQueueIndex.value(), 0);
}

}