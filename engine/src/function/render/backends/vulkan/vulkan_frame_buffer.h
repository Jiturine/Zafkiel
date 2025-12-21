#pragma once
#include "function/render/backends/vulkan/vulkan_image.h"
#include "function/render/frame_buffer.h"
#include "vulkan_command_manager.h"

namespace Zafkiel
{
class VulkanFrameBufferBackend final : public FrameBufferBackend 
{
  public:
    VulkanFrameBufferBackend(const FrameBufferSpecification &spec, const Scope<VulkanDevice> &device);
    
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
    virtual void Resize(uint32_t width, uint32_t height) override;

  private:
    void Invalidate(uint32_t width, uint32_t height);

    std::vector<vk::raii::Framebuffer> frameBuffers;
    std::vector<Observer<Image>> attachments;
    const Observer<const RenderPass> renderPass;
    const Scope<VulkanDevice> &device;
};

class VulkanFrameBufferFactory 
{
  public:
    static Scope<FrameBuffer> Create(const FrameBufferSpecification &spec, const Scope<VulkanDevice> &device);
};

}