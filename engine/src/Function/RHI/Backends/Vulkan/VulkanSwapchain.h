#pragma once
#include "Function/RHI/RHIResources.h"
#include "Function/RHI/Backends/Vulkan/VulkanTexture.h"

namespace Zafkiel 
{
class VulkanDevice;
class PlatformWindow;

class VulkanSwapchainTexture : public VulkanTextureBase
{
  public:
    VulkanSwapchainTexture(uint32 width, uint32 height, vk::Image image, vk::SurfaceFormatKHR surfaceFormat, VulkanDevice &device);
    
    virtual const vk::Image &GetLowLevelImage() override { return image; };

    virtual vk::raii::ImageView &GetImageView() override { return imageView; }

  private:
    vk::Image image;
    vk::raii::ImageView imageView;
};

class VulkanSwapchain final
{
  public:
    VulkanSwapchain(PlatformWindow *window, vk::raii::Instance &instance, VulkanDevice &device, uint32 width, uint32 height);

    vk::raii::SwapchainKHR &GetHandle() { return handle; }

    uint32 AcquireNextImageIndex();

    VulkanSwapchainTexture *GetCurrentAvailableTexture();

    VulkanSwapchainTexture *GetTexture(uint32 index)
    {
        return swapchainTextures[index].get();
    }

    void Present();

    uint32 GetWidth() const { return width; }
    uint32 GetHeight() const { return height; }
    uint32 GetFrameCount() const { return frameCount; }
    
    void Resize(uint32 width, uint32 height);
    void Invalidate();

  private:
    VulkanDevice &device;
    vk::raii::SurfaceKHR surface;

    uint32 width, height;
    uint32 frameCount;
    
    vk::SurfaceFormatKHR surfaceFormat;
    vk::PresentModeKHR presentMode;
    
    std::vector<Ref<VulkanSwapchainTexture>> swapchainTextures;

    std::vector<vk::raii::Fence> fences;
    std::vector<vk::raii::Semaphore> imageAvailableSemaphores;
    std::vector<vk::raii::Semaphore> renderFinishedSemaphores;

    uint32 semaphoreIndex = 0;
    uint32 curImageIndex = 0;

    vk::raii::SwapchainKHR handle;
};

}