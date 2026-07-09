#pragma once
#include <vulkan/vulkan_raii.hpp>
#include "Function/RHI/Backends/Vulkan/VulkanShader.h"
#include "Function/RHI/Backends/Vulkan/VulkanDescriptorManager.h"
#include "Function/RHI/Backends/Vulkan/VulkanRenderPass.h"
#include "Function/RHI/Backends/Vulkan/VulkanCommandList.h"
#include "Function/RHI/Backends/Vulkan/VulkanSwapchain.h"

namespace Zafkiel 
{

class VulkanQueue;
class VulkanRHI;

class VulkanDevice
{
  public:
    VulkanDevice(vk::raii::PhysicalDevice physicalDevice, VulkanRHI &rhi);
  
    vk::raii::PhysicalDevice &GetPhysicalHandle() { return physicalDevice; }
    
    vk::raii::Device &GetHandle() { return device; }

    VulkanGraphicsContext *GetGraphicsContext() { return graphicsContext.get(); }

    VulkanQueue &GetGraphicsQueue() { return *graphicsQueue.get(); }
  
    VulkanShaderRegistry &GetShaderRegistry() { return *shaderRegistry.get(); }

    VulkanDescriptorManager &GetDescriptorManager() { return *descriptorManager.get(); }
  
    VulkanRenderPassManager &GetRenderPassManager() { return *renderPassManager.get(); }

    uint32 GetMinUniformBufferOffsetAlignment() const { return minUniformBufferOffsetAlignment; }

    friend class VulkanSwapchain;

  private:
    vk::raii::PhysicalDevice physicalDevice;

    vk::raii::Device device;
  
    Scope<VulkanGraphicsContext> graphicsContext;
    
    Scope<VulkanQueue> graphicsQueue;
  
    Scope<VulkanShaderRegistry> shaderRegistry;

    Scope<VulkanDescriptorManager> descriptorManager;

    Scope<VulkanRenderPassManager> renderPassManager;

    VulkanQueue *presentQueue;
  
    VulkanRHI &rhi;

    uint32 minUniformBufferOffsetAlignment;
};
}