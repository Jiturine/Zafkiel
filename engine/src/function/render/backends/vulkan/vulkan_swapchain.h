#pragma once
#include <vulkan/vulkan.hpp>
#include "vulkan_render_pass.h"

namespace Zafkiel 
{
class VulkanDevice;
class VulkanPhysicalDevice;

struct VulkanSwapchainSpecification
{
    const Scope<VulkanDevice> &device;
    const Scope<VulkanPhysicalDevice> &physicalDevice;
    const Scope<RenderPass> &renderPass;
    vk::raii::SurfaceKHR &surface;
    uint32_t width;
    uint32_t height;
};

class VulkanSwapchain final
{
  public:
    VulkanSwapchain(const VulkanSwapchainSpecification& spec);

    vk::raii::SwapchainKHR &GetHandle() { return swapchain; }
    vk::raii::Framebuffer &GetFrameBuffer(uint32_t index) { return frameBuffers[index]; }
    uint32_t GetWidth() const { return width; }
    uint32_t GetHeight() const { return height; }
    uint32_t GetFrameCount() const { return frameCount; }
    
    void Resize(uint32_t width, uint32_t height);
    void Invalidate();

  private:
    const Scope<VulkanDevice> &device;
    const Scope<VulkanPhysicalDevice> &physicalDevice;
    const vk::raii::SurfaceKHR &surface;
    Observer<RenderPass> renderPass;

    uint32_t width, height;
    uint32_t frameCount;
    
    vk::SurfaceFormatKHR surfaceFormat;
    vk::PresentModeKHR presentMode;
    
    std::vector<vk::Image> images;
    std::vector<vk::raii::ImageView> imageViews;
    std::vector<vk::raii::Framebuffer> frameBuffers;
    vk::raii::SwapchainKHR swapchain;
};

}