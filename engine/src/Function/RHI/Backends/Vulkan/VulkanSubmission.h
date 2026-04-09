#pragma once
#include "Function/RHI/Backends/Vulkan/VulkanCommandBuffer.h"

namespace Zafkiel
{
enum class VulkanPayloadPhase
{
    Wait,
    Execute,
    Signal,
};

class VulkanPayload
{
  public:
    VulkanPayload() : fence(nullptr) {}
    std::vector<vk::Semaphore> waitSemaphores; // wait before command buffers
    std::vector<VulkanCommandBuffer *> commandBuffers;
    std::vector<vk::Semaphore> signalSemaphores; // signaled after command buffers
    std::vector<vk::PipelineStageFlags> waitStages;
    vk::Fence fence;
};

}