#pragma once
#include "function/render/object_shader_material_template.h"
#include <vulkan/vulkan_raii.hpp>

namespace Zafkiel 
{

class VulkanObjectShaderMaterialTemplateBackend final : public ObjectShaderMaterialTemplateBackend
{
  public:
    VulkanObjectShaderMaterialTemplateBackend(vk::raii::DescriptorSetLayout descriptorSetLayout)
        : descriptorSetLayout(std::move(descriptorSetLayout)) {}
    
    const vk::raii::DescriptorSetLayout &GetDescriptorSetLayout() const { return descriptorSetLayout; }
    vk::raii::DescriptorSetLayout &GetDescriptorSetLayout()  { return descriptorSetLayout; }

  private:
    vk::raii::DescriptorSetLayout descriptorSetLayout;
};

}