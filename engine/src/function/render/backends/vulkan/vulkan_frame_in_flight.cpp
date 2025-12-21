#include "vulkan_frame_in_flight.h"

namespace Zafkiel 
{
VulkanFrameInFlight::VulkanFrameInFlight(const Scope<VulkanCommandManager> &commandManager, const Scope<VulkanDevice> &device)
    : cmdBuf(nullptr), imageAvailableSem(nullptr), renderFinishedSem(nullptr), fence(nullptr), device(device)
{
    cmdBuf = commandManager->CreateOneCommandBuffer();
    imageAvailableSem = device->GetHandle().createSemaphore({});
    renderFinishedSem = device->GetHandle().createSemaphore({});
    vk::FenceCreateInfo fenceCreateInfo;
    fenceCreateInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);
    fence = device->GetHandle().createFence(fenceCreateInfo);
}

}