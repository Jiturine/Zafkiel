#pragma once
#include "function/render/shader_module.h"
#include "function/render/backends/vulkan/vulkan_device.h"

namespace Zafkiel
{

class VulkanShaderModuleBackend final : public ShaderModuleBackend
{
  public:
    VulkanShaderModuleBackend(vk::raii::ShaderModule shaderModule)
        : shaderModule(std::move(shaderModule)) {}
    vk::raii::ShaderModule &GetHandle() { return shaderModule; }
    const vk::raii::ShaderModule &GetHandle() const { return shaderModule; }

  private:
    vk::raii::ShaderModule shaderModule;
};

}
