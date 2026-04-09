#include "Function/RHI/Backends/Vulkan/VulkanDevice.h"
#include "Function/RHI/Backends/Vulkan/VulkanQueue.h"

namespace Zafkiel 
{

VulkanDevice::VulkanDevice(vk::raii::PhysicalDevice tmpPhysicalDevice, vk::raii::SurfaceKHR &surface, VulkanRHI &rhi)
    : physicalDevice(MoveTemp(tmpPhysicalDevice)), device(nullptr), graphicsQueue(nullptr), rhi(rhi)
{
    // 选择队列族
    auto properties = physicalDevice.getQueueFamilyProperties();
    std::optional<uint32> optGraphicsQueueIndex;
    for (int i = 0; i < properties.size(); i++)
    {
        if (properties[i].queueFlags & vk::QueueFlagBits::eGraphics)
            optGraphicsQueueIndex = i;
        if (optGraphicsQueueIndex.has_value())
            break;
    }
    if (!optGraphicsQueueIndex.has_value())
    {
        Log::Error("Queue Family Unavailable!");
    }
    
    minUniformBufferOffsetAlignment = this->physicalDevice.getProperties().limits.minUniformBufferOffsetAlignment;
    
    vk::StructureChain<vk::DeviceCreateInfo, 
                       vk::PhysicalDeviceSwapchainMaintenance1FeaturesEXT, 
                       vk::PhysicalDeviceGraphicsPipelineLibraryFeaturesEXT> deviceChain;
    
    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
    float priorities = 1.0f;

    vk::DeviceQueueCreateInfo queueCreateInfo;
    queueCreateInfo.setQueuePriorities(priorities)
                    .setQueueCount(1)
                    .setQueueFamilyIndex(optGraphicsQueueIndex.value());
    queueCreateInfos.push_back(queueCreateInfo);

    deviceChain.get().setQueueCreateInfos(queueCreateInfo);

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

    device = physicalDevice.createDevice(deviceChain.get());
    
    graphicsQueue = CreateScope<VulkanQueue>(*this, optGraphicsQueueIndex.value());
    
    if (physicalDevice.getSurfaceSupportKHR(graphicsQueue->GetQueueFamilyIndex(), surface))
    {
        presentQueue = graphicsQueue.get();
    }
    else 
    {
        Log::Error("Cannot support present!");
    }
    
    renderPassManager = CreateScope<VulkanRenderPassManager>(*this);

    descriptorManager = CreateScope<VulkanDescriptorManager>(*this);

    graphicsContext = CreateScope<VulkanGraphicsContext>(*graphicsQueue.get(), *this);

    swapchain = CreateScope<VulkanSwapchain>(*this, surface, 1280, 720);

    shaderRegistry = CreateScope<VulkanShaderRegistry>();
}

}