#pragma once
#include "vulkan_descriptor_manager.h"
#include "function/render/global_render_resource.h"

namespace Zafkiel 
{
class VulkanGlobalRenderResourceFactory
{
  public:
    static Scope<GlobalRenderResource> Create(const Path &path, const Scope<VulkanDevice> &device, const Scope<VulkanPhysicalDevice> &physicalDevice, const Scope<VulkanDescriptorManager> &descriptorManager, uint32_t frameCount);
};

}