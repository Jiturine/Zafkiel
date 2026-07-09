#pragma once
#include "Function/RHI/RHIResources.h"
#include "Function/RHI/Backends/Vulkan/VulkanSwapchain.h"

namespace Zafkiel
{

class PlatformWindow;

class VulkanViewport : public RHIViewport
{
  public:
    VulkanViewport(PlatformWindow *window, vk::raii::Instance &instance, VulkanDevice &device);

    VulkanSwapchain *GetSwapchain() { return swapchain.get(); }

    virtual RHITexture *GetBackendTexture() override { return swapchain->GetCurrentAvailableTexture(); }

    virtual void Resize(uint32 width, uint32 height) override;

  private:
    Scope<VulkanSwapchain> swapchain;

    vk::raii::SurfaceKHR surface;
};

}