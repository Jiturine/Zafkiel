#include "function/render/backends/vulkan/vulkan_texture.h"
#include "function/render/backends/vulkan/vulkan_buffer.h"
#include "vulkan/vulkan.hpp"
#include "function/render/backends/vulkan/vulkan_image.h"

namespace Zafkiel
{
vk::SamplerAddressMode TextureWrapToVulkanAddressMode(TextureWrap wrap)
{
    switch (wrap)
    {
        using enum TextureWrap;
    case Repeat: return vk::SamplerAddressMode::eRepeat;
    case Clamp: return vk::SamplerAddressMode::eClampToBorder;
    default:
        Log::Error("Unsupported TextureWrap!");
        return vk::SamplerAddressMode::eRepeat;
    }
}

vk::Filter FilterTypeToVulkanType(TextureFilter filter)
{
    switch (filter) 
    {
        using enum TextureFilter;
    case Nearest: return vk::Filter::eNearest;
    case Linear: return vk::Filter::eLinear;
    default:
        Log::Error("Unsupported TextureFilter!");
        return vk::Filter::eNearest;
    }
}

}