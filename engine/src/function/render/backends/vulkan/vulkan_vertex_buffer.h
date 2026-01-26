#pragma once
#include "function/render/backends/vulkan/vulkan_command_manager.h"
#include "function/render/vertex_buffer.h"
#include <vulkan/vulkan.hpp>
#include "function/render/backends/vulkan/vulkan_buffer.h"

namespace Zafkiel
{

vk::Format ShaderDataTypeToVulkanFormat(ShaderFundamentalType type);

class VulkanVertexBufferBackend final : public VertexBufferBackend
{
  public:
    VulkanVertexBufferBackend(Scope<VulkanBuffer> buffer)
        : buffer(std::move(buffer)) {}
    vk::raii::Buffer &GetHandle() { return buffer->buffer; }
    const vk::raii::Buffer &GetHandle() const { return buffer->buffer; }
  private:
    Scope<VulkanBuffer> buffer;
};

}