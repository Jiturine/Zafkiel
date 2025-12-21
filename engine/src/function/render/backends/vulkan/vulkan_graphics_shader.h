#pragma once
#include "vulkan_descriptor_manager.h"
#include "vulkan_fragment_module.h"
#include "vulkan_vertex_module.h"
#include "function/render/graphics_shader.h"
#include "platform/filesystem/filesystem.h"
#include "vulkan_device.h"
#include "vulkan_shader.h"

namespace Zafkiel
{

class VulkanGraphicsShaderBackend final : public GraphicsShaderBackend
{
  private:
    // Observer<GraphicsShader> graphicsShader;

};

class VulkanGraphicsShaderFactory : public GraphicsShaderFactory<VulkanGraphicsShaderFactory>
{
  public:
    static Scope<GraphicsShader> Create(const Path &path, const Scope<VulkanDevice> &device, const Scope<VulkanDescriptorManager> &descriptorManager);
};


}
