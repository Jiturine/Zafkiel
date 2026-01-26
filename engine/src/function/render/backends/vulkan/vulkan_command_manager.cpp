#include "function/render/backends/vulkan/vulkan_command_manager.h"

namespace Zafkiel 
{
VulkanCommandManager::VulkanCommandManager(Borrow<VulkanDevice> device, Borrow<VulkanPhysicalDevice> physicalDevice)
    : commandPool(nullptr), device(device)
{
    vk::CommandPoolCreateInfo createInfo;
    createInfo.setQueueFamilyIndex(physicalDevice->GetGraphicsQueueIndex())
              .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
    commandPool = device->GetHandle().createCommandPool(createInfo);
}

std::vector<vk::raii::CommandBuffer> VulkanCommandManager::CreateCommandBuffers(uint32_t count) const
{
    vk::CommandBufferAllocateInfo allocInfo;
    allocInfo.setCommandPool(commandPool)
             .setCommandBufferCount(count)
             .setLevel(vk::CommandBufferLevel::ePrimary);

    return device->GetHandle().allocateCommandBuffers(allocInfo);
}

vk::raii::CommandBuffer VulkanCommandManager::CreateOneCommandBuffer() const 
{
    return std::move(CreateCommandBuffers(1)[0]);
}

}