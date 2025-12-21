#pragma once
#include "vulkan_shader_module.h"
#include "function/render/fragment_module.h"
#include "vulkan_device.h"

namespace Zafkiel 
{

class VulkanFragmentModuleBackend final : public FragmentModuleBackend
{
  public: 
    VulkanFragmentModuleBackend(Buffer codeBuffer, const Scope<VulkanDevice> &device, const Scope<VulkanShaderModuleBackend> &shaderModuleBackend);
};

class VulkanFragmentModuleFactory
{
  public:
    static Scope<FragmentModule> Create(Buffer buffer, const Scope<VulkanDevice> &device);
};

}