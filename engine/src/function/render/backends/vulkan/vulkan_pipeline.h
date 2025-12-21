#pragma once
#include <vulkan/vulkan_raii.hpp>
#include "function/render/pipeline.h"

namespace Zafkiel 
{

vk::PipelineBindPoint PipelineTypeToVulkanBindPoint(PipelineType type);

class VulkanPipelineBackend final : public PipelineBackend
{
  public:
    VulkanPipelineBackend() : pipeline(nullptr), pipelineLayout(nullptr) {}
    vk::raii::Pipeline &GetPipeline() { return pipeline; }
    const vk::raii::Pipeline &GetPipeline() const { return pipeline; }
    vk::raii::PipelineLayout &GetPipelineLayout() { return pipelineLayout; }
    const vk::raii::PipelineLayout &GetPipelineLayout() const { return pipelineLayout; }
    
    friend class VulkanGraphicsPipelineBackend;
  private:
    vk::raii::Pipeline pipeline;
    vk::raii::PipelineLayout pipelineLayout;
};

}