#pragma once
#include <vulkan/vulkan.hpp>
#include "function/render/backends/vulkan/vulkan_render_pass.h"

namespace Zafkiel 
{
class VulkanDevice;
class VulkanPhysicalDevice;

struct VulkanSwapchainSpecification
{
    Borrow<VulkanDevice> device;
    Borrow<VulkanPhysicalDevice> physicalDevice;
    Borrow<RenderPass> renderPass;
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
    Borrow<VulkanDevice> device;
    Borrow<VulkanPhysicalDevice> physicalDevice;
    const vk::raii::SurfaceKHR &surface;
    Borrow<RenderPass> renderPass;

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