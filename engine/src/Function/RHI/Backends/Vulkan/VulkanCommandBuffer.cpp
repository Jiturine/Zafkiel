#include "Function/RHI/Backends/Vulkan/VulkanCommandBuffer.h"
#include "Function/RHI/Backends/Vulkan/VulkanDevice.h"
#include "Function/RHI/Backends/Vulkan/VulkanQueue.h"

namespace Zafkiel 
{

    
VulkanCommandPool::VulkanCommandPool(VulkanDevice &device, VulkanQueue &queue)
    : handle(nullptr), device(device), queue(queue)
{
    vk::CommandPoolCreateInfo createInfo;
    createInfo.setQueueFamilyIndex(queue.GetQueueFamilyIndex())
              .setFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);
    handle = device.GetHandle().createCommandPool(createInfo);
}

VulkanCommandBuffer *VulkanCommandPool::CreateCommandBuffer()
{
	if (freeCommandBuffers.size())
	{
		Scope<VulkanCommandBuffer> cmdBuffer = MoveTemp(freeCommandBuffers.back());
		freeCommandBuffers.pop_back();
        cmdBuffer->AllocMemory();
		commandBuffers.push_back(MoveTemp(cmdBuffer));
		return commandBuffers.back().get();
	}

	Scope<VulkanCommandBuffer> cmdBuffer = CreateScope<VulkanCommandBuffer>(device, *this);
    cmdBuffer->AllocMemory();
	commandBuffers.push_back(MoveTemp(cmdBuffer));
	return commandBuffers.back().get();
}

void VulkanCommandBuffer::AllocMemory()
{
    vk::CommandBufferAllocateInfo allocInfo;
    allocInfo.setCommandPool(commandPool.GetHandle())
             .setCommandBufferCount(1)
             .setLevel(vk::CommandBufferLevel::ePrimary);

    handle = MoveTemp(device.GetHandle().allocateCommandBuffers(allocInfo)[0]);

    state = VulkanCommandBufferState::ReadyForBegin;
}

void VulkanCommandBuffer::FreeMemory()
{
    handle = nullptr;

    state = VulkanCommandBufferState::NotAllocated;
}

void VulkanCommandBuffer::Begin()
{
    vk::CommandBufferBeginInfo beginInfo;
    beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    handle.begin(beginInfo);

    state = VulkanCommandBufferState::IsInsideBegin;
}

void VulkanCommandBuffer::End()
{
    handle.end();

    state = VulkanCommandBufferState::HasEnded;
}

void VulkanCommandBuffer::BeginRenderPass(const vk::RenderPassBeginInfo &beginInfo)
{
    handle.beginRenderPass(beginInfo, vk::SubpassContents::eInline);

    state = VulkanCommandBufferState::IsInsideRenderPass;
}

void VulkanCommandBuffer::EndRenderPass()
{
    handle.endRenderPass();

    state = VulkanCommandBufferState::IsInsideBegin;
}

void VulkanCommandBuffer::SetSubmitted()
{
    state = VulkanCommandBufferState::Submitted;
}

}