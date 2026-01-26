#pragma once
#include "function/render/backends/vulkan/vulkan_pipeline.h"
#include "function/render/graphics_pipeline.h"

namespace Zafkiel
{

vk::PrimitiveTopology PrimitiveTopologyToVulkanType(PrimitiveTopology type);
vk::CullModeFlagBits CullModeToVulkanType(CullMode mode);
vk::FrontFace FrontFaceToVulkanType(FrontFace type);
vk::PolygonMode PolygonModeToVulkanType(PolygonMode mode);

}