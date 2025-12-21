#pragma once
#include "vulkan_uniform_buffer.h"
#include "function/render/material.h"
#include "vulkan_descriptor_manager.h"
#include "vulkan_shader.h"

namespace Zafkiel
{
class VulkanMaterialFactory
{
  public:
    static Scope<Material> Create(const MaterialSpecification &spec, const Scope<VulkanDevice> &device, const Scope<VulkanPhysicalDevice> &physicalDevice, const Scope<VulkanDescriptorManager> &descriptorManager, uint32_t frameCount);
};
}