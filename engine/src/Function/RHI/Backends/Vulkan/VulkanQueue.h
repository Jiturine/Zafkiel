#pragma once
#include "Function/RHI/Backends/Vulkan/VulkanCommandBuffer.h"
#include "Function/RHI/Backends/Vulkan/VulkanSubmission.h"

namespace Zafkiel
{

class VulkanDevice;

class VulkanQueue
{
  public:
    VulkanQueue(VulkanDevice &device, uint32 queueFamilyIndex);

    vk::raii::Queue &GetHandle() { return handle; }
  
    uint32 GetQueueFamilyIndex() const { return queueFamilyIndex; }
  
    Scope<VulkanCommandPool> AcquireCommandPool();
  
    void ReleaseCommandPool(Scope<VulkanCommandPool> pool);

    void EnqueuePayload(Scope<VulkanPayload> payload);

    void SubmitPayloads();

  private:
    VulkanDevice &device;

    std::vector<Scope<VulkanCommandPool>> availableCommandPools;

    std::vector<Scope<VulkanPayload>> pendingSubmissionPayloads;

    uint32 queueFamilyIndex;

    vk::raii::Queue handle;
};

}