#pragma once
#include "function/render/pipeline.h"
#include "function/render/render_pass.h"
#include "function/render/backends/vulkan/vulkan_device.h"
#include "function/render/backends/vulkan/vulkan_frame_buffer.h"

namespace Zafkiel
{

vk::ClearValue ClearValueToVulkanType(ClearValue value);
  
class VulkanRenderPassBackend final : public RenderPassBackend
{
  public:
    VulkanRenderPassBackend(vk::raii::RenderPass renderPass) : renderPass(std::move(renderPass)) {}
    
    vk::raii::RenderPass &GetHandle() { return renderPass; }
    const vk::raii::RenderPass &GetHandle() const { return renderPass; }

  private:
    vk::raii::RenderPass renderPass;
};
}