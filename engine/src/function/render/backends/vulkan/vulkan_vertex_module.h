#pragma once
#include "vulkan_shader_module.h"
#include "function/render/vertex_module.h"
#include "vulkan_device.h"

namespace Zafkiel
{

class VulkanVertexModuleBackend final : public VertexModuleBackend
{
  public:
    VulkanVertexModuleBackend(Buffer codeBuffer, const Scope<VulkanDevice> &device, const Scope<VulkanShaderModuleBackend> &shaderModuleBackend);
};

class VulkanVertexModuleFactory
{
  public:
    static Scope<VertexModule> Create(Buffer codeBuffer, const Scope<VulkanDevice> &device);
};

}
