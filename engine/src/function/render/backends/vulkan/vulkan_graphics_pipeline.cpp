#include "function/render/backends/vulkan/vulkan_graphics_pipeline.h"

namespace Zafkiel
{

vk::PrimitiveTopology PrimitiveTopologyToVulkanType(PrimitiveTopology type)
{
    switch (type)
    {
        using enum PrimitiveTopology;
	case Points: return vk::PrimitiveTopology::ePointList;
	case Lines: return vk::PrimitiveTopology::eLineList;
	case Triangles: return vk::PrimitiveTopology::eTriangleList;
	case LineStrip: return vk::PrimitiveTopology::eLineStrip;
	case TriangleStrip: return vk::PrimitiveTopology::eTriangleStrip;
	case TriangleFan: return vk::PrimitiveTopology::eTriangleFan;
    default:
        Log::Error("Unknown Primitive Topology!");
        return vk::PrimitiveTopology::ePointList;
    }
}

vk::CullModeFlagBits CullModeToVulkanType(CullMode mode)
{
    switch (mode)
    {
        using enum CullMode;
    case Front: return vk::CullModeFlagBits::eFront;
    case Back: return vk::CullModeFlagBits::eBack;
    case FrontAndBack: return vk::CullModeFlagBits::eFrontAndBack;
    case None: return vk::CullModeFlagBits::eNone;
    default: return vk::CullModeFlagBits::eNone;
    }
}

vk::FrontFace FrontFaceToVulkanType(FrontFace type)
{
    switch (type)
    {
        using enum FrontFace;
    case CounterClockWise: return vk::FrontFace::eCounterClockwise;
    case ClockWise: return vk::FrontFace::eClockwise;
    default: return vk::FrontFace::eCounterClockwise;
    }
}

vk::PolygonMode PolygonModeToVulkanType(PolygonMode mode)
{
    switch (mode)
    {
        using enum PolygonMode;
    case Fill: return vk::PolygonMode::eFill;
    case Wireframe: return vk::PolygonMode::eLine;
    default: return vk::PolygonMode::eFill;
    }
}
}
