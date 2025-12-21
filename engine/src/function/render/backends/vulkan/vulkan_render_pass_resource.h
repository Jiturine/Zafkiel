#pragma once
#include "vulkan_descriptor_manager.h"
#include "function/render/render_pass_resource.h"

namespace Zafkiel 
{
class VulkanRenderPassResourceFactory
{
  public:
    static Scope<RenderPassResource> Create(const Path &path, const Scope<VulkanDevice> &device, const Scope<VulkanPhysicalDevice> &physicalDevice, const Scope<VulkanDescriptorManager> &descriptorManager, uint32_t frameCount);
};

}