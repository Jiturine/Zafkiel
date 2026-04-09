#pragma once
#include <vulkan/vulkan_raii.hpp>
#include "Function/RHI/RHIResources.h"

namespace Zafkiel
{

enum class VulkanCommandBufferState
{
    ReadyForBegin,
    IsInsideBegin,
    IsInsideRenderPass,
    HasEnded,
    Submitted,
    NotAllocated,
    NeedReset,
};

class VulkanCommandPool;
class VulkanDevice;
class VulkanQueue;

class VulkanCommandBuffer 
{
  public:
    VulkanCommandBuffer(VulkanDevice &device, VulkanCommandPool &pool)
        : device(device), commandPool(pool), handle(nullptr) {}
    
    vk::raii::CommandBuffer &GetHandle()
    {
        return handle;
    }
    
    bool IsAvailable() const 
    {
        return state == VulkanCommandBufferState::ReadyForBegin || state == VulkanCommandBufferState::NeedReset;
    }

    bool IsInsideRenderPass() const
    {
        return state == VulkanCommandBufferState::IsInsideRenderPass;
    }

    bool IsOutsideRenderPass() const
    {
        return state == VulkanCommandBufferState::IsInsideBegin;
    }

    bool HasBegun() const
    {
        return state == VulkanCommandBufferState::IsInsideBegin || state == VulkanCommandBufferState::IsInsideRenderPass;
    }

    bool HasEnded() const
    {
        return state == VulkanCommandBufferState::HasEnded;
    }

    bool IsSubmitted() const
    {
        return state == VulkanCommandBufferState::Submitted;
    }

    bool IsAllocated() const
    {
        return state != VulkanCommandBufferState::NotAllocated;
    }

    void Begin();

    void End();
  
    void BeginRenderPass(const vk::RenderPassBeginInfo &beginInfo);

    void EndRenderPass();
    
    void AllocMemory();

    void FreeMemory();

    void SetSubmitted();

  private:
    VulkanDevice &device;
    VulkanCommandPool &commandPool;
    VulkanCommandBufferState state = VulkanCommandBufferState::NotAllocated;
    vk::raii::CommandBuffer handle;
};

class VulkanCommandPool final
{
  public:
    VulkanCommandPool(VulkanDevice &device, VulkanQueue &queue);

    vk::raii::CommandPool &GetHandle() { return handle; }

    VulkanQueue &GetQueue() { return queue; }

    VulkanCommandBuffer *CreateCommandBuffer();
    
    friend class VulkanCommandContext;

  private:
    VulkanDevice &device;
    VulkanQueue &queue;
    vk::raii::CommandPool handle;
    std::vector<Scope<VulkanCommandBuffer>> commandBuffers;
    std::vector<Scope<VulkanCommandBuffer>> freeCommandBuffers;
};

}