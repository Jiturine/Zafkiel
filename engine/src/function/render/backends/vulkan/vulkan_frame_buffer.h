#pragma once
#include "function/render/backends/vulkan/vulkan_image.h"
#include "function/render/backends/vulkan/vulkan_render_pass.h"
#include "function/render/frame_buffer.h"
#include "function/render/backends/vulkan/vulkan_command_manager.h"
#include "function/render/render_registry.h"

namespace Zafkiel
{
class VulkanContext;

class VulkanFrameBufferBackend final : public FrameBufferBackend
{
  public:
    VulkanFrameBufferBackend(std::vector<vk::raii::Framebuffer> frameBuffers)
        : frameBuffers(std::move(frameBuffers)) {}

    vk::raii::Framebuffer &GetFrameBuffer(uint32_t index)
    {
        if (frameBuffers.size() > 1)
            return frameBuffers[index];
        else
            return frameBuffers[0];
    }
    const vk::raii::Framebuffer &GetFrameBuffer(uint32_t index) const
    {
        if (frameBuffers.size() > 1)
            return frameBuffers[index];
        else
            return frameBuffers[0];
    }

  private:
    std::vector<vk::raii::Framebuffer> frameBuffers;
};

}