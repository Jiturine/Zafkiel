#pragma once
#include "function/render/shader_material_template.h"
#include "function/render/backends/vulkan/vulkan_device.h"

namespace Zafkiel 
{

class VulkanShaderMaterialTemplateBackend final : public ShaderMaterialTemplateBackend
{
  public:
    VulkanShaderMaterialTemplateBackend(vk::raii::DescriptorSetLayout descriptorSetLayout)
        : descriptorSetLayout(std::move(descriptorSetLayout)) {}
    
    const vk::raii::DescriptorSetLayout &GetDescriptorSetLayout() const { return descriptorSetLayout; }
    vk::raii::DescriptorSetLayout &GetDescriptorSetLayout() { return descriptorSetLayout; }

  private:
    vk::raii::DescriptorSetLayout descriptorSetLayout;
};

}