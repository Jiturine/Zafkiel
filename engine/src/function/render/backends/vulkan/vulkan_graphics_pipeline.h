#pragma once
#include "vulkan_pipeline.h"
#include "function/render/graphics_pipeline.h"
#include "vulkan_device.h"

namespace Zafkiel
{

vk::PrimitiveTopology PrimitiveTopologyToVulkanType(PrimitiveTopology type);
vk::CullModeFlagBits CullModeToVulkanType(CullMode mode);
vk::FrontFace FrontFaceToVulkanType(FrontFace type);
vk::PolygonMode PolygonModeToVulkanType(PolygonMode mode);

class VulkanRenderPass;

class VulkanGraphicsPipelineBackend final : public GraphicsPipelineBackend
{
  public:
    VulkanGraphicsPipelineBackend(const GraphicsPipelineSpecification &spec, const Scope<VulkanDevice> &device, const Scope<VulkanPipelineBackend> &pipelineBackend);
};

class VulkanGraphicsPipelineFactory
{
  public:
    static Scope<GraphicsPipeline> Create(const GraphicsPipelineSpecification &spec, const Scope<VulkanDevice> &device);
};

}