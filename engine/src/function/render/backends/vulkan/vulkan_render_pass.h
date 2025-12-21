#pragma once
#include "core/base/memory.h"
#include "function/render/pipeline.h"
#include "function/render/render_pass.h"
#include <vulkan/vulkan.hpp>
#include "vulkan_device.h"
#include "vulkan_frame_buffer.h"

namespace Zafkiel
{
class VulkanRenderPassBackend final : public RenderPassBackend
{
  public:
    VulkanRenderPassBackend(const RenderPassSpecification &spec, const Scope<VulkanDevice> &device);
    
    vk::raii::RenderPass &GetHandle() { return renderPass; }
    const vk::raii::RenderPass &GetHandle() const { return renderPass; }

    friend class VulkanFrameBuffer;

  private:
    vk::raii::RenderPass renderPass;
    const Scope<VulkanDevice> &device;
};

class VulkanRenderPassFactory
{
  public:
    static Scope<RenderPass> Create(const RenderPassSpecification &spec, const Scope<VulkanDevice> &device);
};

}