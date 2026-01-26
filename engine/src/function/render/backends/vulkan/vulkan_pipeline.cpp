#include "function/render/backends/vulkan/vulkan_pipeline.h"

namespace Zafkiel 
{

vk::PipelineBindPoint PipelineTypeToVulkanBindPoint(PipelineType type)
{
    switch (type)
    {
        using enum PipelineType;
    case Graphics: return vk::PipelineBindPoint::eGraphics;
    case Compute: return vk::PipelineBindPoint::eCompute;
    default:
        Log::Error("Unsupported Pipeline Type!");
        return vk::PipelineBindPoint::eGraphics;
    }
}
}