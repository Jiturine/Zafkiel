#include "function/render/backends/vulkan/vulkan_render_pass.h"

namespace Zafkiel 
{

vk::ClearValue ClearValueToVulkanType(ClearValue value)
{
    vk::ClearValue result;
    if (value.type == AttachmentType::Color)
    {
        switch (value.format)
        {
            using enum ImageFormat;
        case R8:
        case R8_sRGB:
            result.setColor(vk::ClearColorValue(std::array<float, 4>{value.floatValue, 0, 0, 0}));
            break;
        case RG8:
        case RG8_sRGB:
            result.setColor(vk::ClearColorValue(std::array<float, 4>{value.vec2Value.x, value.vec2Value.y, 0, 0}));
            break;
        case BGR8:
        case RGB8:
        case RGB8_sRGB:
        case BGR8_sRGB:
        case RGB16F:
            result.setColor(vk::ClearColorValue(std::array<float, 4>{value.vec3Value.x, value.vec3Value.y, value.vec3Value.z, 0}));
            break;
        case BGRA8:
        case RGBA8:
        case RGBA8_sRGB:
        case BGRA8_sRGB:
        case RGBA16F:
        case RGBA32F:
            result.setColor(vk::ClearColorValue(std::array<float, 4>{value.vec4Value.x, value.vec4Value.y, value.vec4Value.z, value.vec4Value.w}));
            break;
        case R32UI:
            result.setColor(vk::ClearColorValue(std::array<uint32_t, 4>{value.uintValue, 0, 0, 0}));
            break;
        case R32F:
            result.setColor(vk::ClearColorValue(std::array<float, 4>{value.floatValue, 0, 0, 0}));
            break;
        default:
            Log::Error("Unsupported Clear Color Value!");
            break;
        }
    }
    else if (value.type == AttachmentType::DepthStencil)
    {
        result.setDepthStencil(vk::ClearDepthStencilValue{value.floatValue, value.uintValue});
    }
    else if (value.type == AttachmentType::Depth)
    {
        result.setDepthStencil(vk::ClearDepthStencilValue{value.floatValue, {}});
    }
    else if (value.type == AttachmentType::Stencil)
    {
        result.setDepthStencil(vk::ClearDepthStencilValue{{}, value.uintValue});
    }
    else
    {
        Log::Error("Unknown ClearValue Format!");
    }
    return result;
}


    
}