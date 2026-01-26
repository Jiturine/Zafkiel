#pragma once
#include "function/render/index_buffer.h"
#include "function/render/backends/vulkan/vulkan_buffer.h"

namespace Zafkiel 
{
class VulkanIndexBufferBackend final : public IndexBufferBackend
{
  public:
    VulkanIndexBufferBackend(Scope<VulkanBuffer> buffer)
        : buffer(std::move(buffer)) {}
    vk::raii::Buffer &GetHandle() { return buffer->buffer; }
    const vk::raii::Buffer &GetHandle() const { return buffer->buffer; }

  private:
    Scope<VulkanBuffer> buffer;
};

}