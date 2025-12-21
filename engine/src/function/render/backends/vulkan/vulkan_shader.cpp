#include "vulkan_shader.h"
#include "core/base/buffer.h"
#include <spirv_cross/spirv_cross.hpp>

namespace Zafkiel
{

vk::ShaderStageFlagBits ShaderStageToVulkanType(ShaderStage stage)
{
    switch (stage)
    {
        using enum ShaderStage;
    case Vertex: return vk::ShaderStageFlagBits::eVertex;
    case Fragment: return vk::ShaderStageFlagBits::eFragment;
    case Geometry: return vk::ShaderStageFlagBits::eGeometry;
    default:
        Log::Error("Unsupported Shader Stage!");
        return vk::ShaderStageFlagBits::eAll;
    }
}


}