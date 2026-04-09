#include "Function/RHI/Backends/Vulkan/VulkanQueue.h"
#include "Function/RHI/Backends/Vulkan/VulkanDevice.h"

namespace Zafkiel 
{

VulkanQueue::VulkanQueue(VulkanDevice &device, uint32 queueFamilyIndex)
    : queueFamilyIndex(queueFamilyIndex), device(device), handle(nullptr)
{
    handle = device.GetHandle().getQueue(queueFamilyIndex, 0);
}

Scope<VulkanCommandPool> VulkanQueue::AcquireCommandPool()
{
	if (availableCommandPools.empty() == false)
	{
		auto commandPool = MoveTemp(availableCommandPools.back());
        availableCommandPools.pop_back();
        return commandPool;
	}
	return CreateScope<VulkanCommandPool>(device, *this);
}

void VulkanQueue::ReleaseCommandPool(Scope<VulkanCommandPool> pool)
{
    if (&pool->GetQueue() != this)
    {
        Log::Error("CommandPool and queue cannot match!");
    }
	availableCommandPools.push_back(MoveTemp(pool));
}

void VulkanQueue::EnqueuePayload(Scope<VulkanPayload> payload)
{
    pendingSubmissionPayloads.push_back(MoveTemp(payload));
}

void VulkanQueue::SubmitPayloads()
{
    for (auto &payload : pendingSubmissionPayloads)
    {
        vk::SubmitInfo submitInfo;
        std::vector<vk::CommandBuffer> cmdBufs;
        for (auto cmdBuf : payload->commandBuffers)
        {
            cmdBufs.push_back(*cmdBuf->GetHandle());
            cmdBuf->SetSubmitted();
        }
        submitInfo.setCommandBuffers(cmdBufs)
                  .setWaitSemaphores(payload->waitSemaphores)
                  .setWaitDstStageMask(payload->waitStages)
                  .setSignalSemaphores(payload->signalSemaphores);

        handle.submit(submitInfo, payload->fence);
    }
    pendingSubmissionPayloads.clear();
}

}