#pragma once
#include "function/render/shader_module.h"
#include <vulkan/vulkan_raii.hpp>

namespace Zafkiel
{

class VulkanShaderModuleBackend final : public ShaderModuleBackend
{
  public:
    VulkanShaderModuleBackend() : shaderModule(nullptr) {}
    vk::raii::ShaderModule &GetHandle() { return shaderModule; }
    const vk::raii::ShaderModule &GetHandle() const { return shaderModule; }

    friend class VulkanVertexModuleBackend;
    friend class VulkanFragmentModuleBackend;
  private:
    vk::raii::ShaderModule shaderModule;
};

}
