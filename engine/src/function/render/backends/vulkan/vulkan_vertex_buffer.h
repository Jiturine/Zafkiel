#pragma once
#include "vulkan_command_manager.h"
#include "function/render/vertex_buffer.h"
#include <vulkan/vulkan.hpp>
#include "vulkan_buffer.h"

namespace Zafkiel
{

vk::Format ShaderDataTypeToVulkanFormat(ShaderFundamentalType type);

class VulkanVertexBufferBackend : public VertexBufferBackend
{
  public:
    VulkanVertexBufferBackend(const float *vertices, uint32_t size, const Scope<VulkanDevice> &device, const Scope<VulkanPhysicalDevice> &physicalDevice, const Scope<VulkanCommandManager> &commandManager);
    vk::raii::Buffer &GetHandle() { return buffer->buffer; }
    const vk::raii::Buffer &GetHandle() const { return buffer->buffer; }
  private:
    Scope<VulkanBuffer> buffer;
};

class VulkanVertexBufferFactory
{
  public:
    static Scope<VertexBuffer> Create(const float *vertices, uint32_t size, const Scope<VulkanDevice> &device, const Scope<VulkanPhysicalDevice> &physicalDevice, const Scope<VulkanCommandManager> &commandManager);
};

}