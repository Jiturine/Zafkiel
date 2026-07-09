#include "Function/RHI/Backends/Vulkan/VulkanViewport.h"
#include "Platform/PlatformWindow/PlatformWindow.h"
#include <SDL3/SDL_vulkan.h>

namespace Zafkiel
{

VulkanViewport::VulkanViewport(PlatformWindow *window, vk::raii::Instance &instance, VulkanDevice &device)
    : surface(nullptr)
{
    swapchain = CreateScope<VulkanSwapchain>(window, instance, device, window->GetWidth(), window->GetHeight());
}

void VulkanViewport::Resize(uint32 width, uint32 height)
{
    swapchain->Resize(width, height);
}

}