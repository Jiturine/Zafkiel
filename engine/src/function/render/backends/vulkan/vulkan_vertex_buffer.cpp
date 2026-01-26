#include "function/render/backends/vulkan/vulkan_vertex_buffer.h"

namespace Zafkiel
{

vk::Format ShaderDataTypeToVulkanFormat(ShaderFundamentalType type)
{
    switch (type)
    {
        using enum ShaderReflection::FundamentalKind;
    case Float2:
        return vk::Format::eR32G32Sfloat;
    case Float3:
        return vk::Format::eR32G32B32Sfloat;
    case Float4:
        return vk::Format::eR32G32B32A32Sfloat;
    default:
        Log::Error("Failed to Convert to Vulkan Format!");
        return vk::Format::eUndefined;
    }
}

}