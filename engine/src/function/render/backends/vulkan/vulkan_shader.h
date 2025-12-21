#pragma once
#include "function/render/shader.h"
#include "function/render/shader_module.h"
#include "vulkan_device.h"
#include "vulkan_vertex_module.h"
#include "vulkan_fragment_module.h"
#include "vulkan_descriptor_manager.h"
#include "platform/filesystem/filesystem.h"

namespace Zafkiel
{

vk::ShaderStageFlagBits ShaderStageToVulkanType(ShaderStage stage);

class VulkanShaderBackend final : public ShaderBackend
{
  public:
    VulkanShaderBackend(const Scope<VulkanDevice> &device, const Scope<VulkanDescriptorManager> &descriptorManager)
        : device(device), descriptorManager(descriptorManager) {}

    virtual ~VulkanShaderBackend() = default;
    
    friend class VulkanGraphicsShaderFactory;

    // const std::vector<vk::raii::DescriptorSetLayout> &GetDescriptorSetLayouts() const { return descriptorSetLayouts; }
    // std::vector<vk::raii::DescriptorSetLayout> &GetDescriptorSetLayouts() { return descriptorSetLayouts; }
    // const vk::raii::DescriptorSetLayout &GetDescriptorSetLayout(size_t setIndex) const { return descriptorSetLayouts[setIndex]; }

  private:
    std::vector<vk::DescriptorSetLayoutBinding> bindings;
    // std::vector<vk::raii::DescriptorSetLayout> descriptorSetLayouts;
    
    const Scope<VulkanDevice> &device;
    const Scope<VulkanDescriptorManager> &descriptorManager;
};

}
