#pragma once
#include "Function/RHI/RHIResources.h"
#include <vulkan/vulkan_raii.hpp>

namespace Zafkiel 
{

class VulkanDevice;
class VulkanUniformBuffer;
class VulkanTexture;
class VulkanBuffer;
class VulkanCommandBuffer;

vk::PipelineBindPoint PipelineTypeToVulkanBindPoint(PipelineType type);

vk::PrimitiveTopology PrimitiveTopologyToVulkanType(PrimitiveTopology type);

vk::CullModeFlagBits CullModeToVulkanType(CullMode mode);

vk::FrontFace FrontFaceToVulkanType(FrontFace type);

vk::PolygonMode PolygonModeToVulkanType(PolygonMode mode);

struct VulkanWriteDescriptorContainer
{
    VulkanWriteDescriptorContainer(uint32 bufferCount, uint32 imageCount)
    {
        bufferInfos.reserve(bufferCount);
        imageInfos.reserve(imageCount);
    }
    std::vector<vk::DescriptorBufferInfo> bufferInfos;
    std::vector<vk::DescriptorImageInfo> imageInfos;
};

class VulkanPipeline;

class VulkanPipelineState
{
  public:
    VulkanPipelineState(const std::vector<RHIShader *> &shaders, VulkanPipeline &pipeline, VulkanDevice &device);

    void SetDynamicOffset(uint32 set, uint32 binding, uint32 offset);

    void SetUniformBuffer(uint32 set, uint32 binding, VulkanBuffer *uniformBuffer);

    void SetTexture(uint32 set, uint32 binding, VulkanTexture *texture);

    void ClearResources();

    void UpdateDescriptorSets(const std::vector<vk::DescriptorSet> &descriptorSets);

    void BindDescriptorSets(VulkanCommandBuffer &commandBuffer);

    bool IsDirty() const { return resourceDirty; }

    const std::vector<RHIShader *> &GetShaders() const { return shaders; }

  private:
    std::vector<vk::WriteDescriptorSet> writeInfos;
    std::vector<vk::WriteDescriptorSet *> writers;
    std::vector<VulkanWriteDescriptorContainer> writeDescriptorContainers;

    std::vector<std::vector<std::optional<VulkanBuffer *>>> currentBufferInfos;
    std::vector<std::vector<std::optional<VulkanTexture *>>> currentImageInfos;

    std::vector<uint32> dynamicOffsets;

    // (set, binding) -> dynamicOffset数组的索引
    std::map<std::pair<uint32, uint32>, uint32> dynamicOffsetMap;

    // (set, binding) -> uniform block的大小（用于设置descriptor range）
    std::map<std::pair<uint32, uint32>, uint32> uniformBlockSizes;

    bool resourceDirty = true;

    VulkanDevice &device;

    VulkanPipeline &pipeline;

    std::vector<RHIShader *> shaders;
};

// 长期静态对象
class VulkanPipeline
{
  public:
    VulkanPipeline();

    vk::raii::PipelineLayout &GetPipelineLayout() { return pipelineLayout; }

    void SetUniformBuffer(uint32 set, uint32 binding, VulkanBuffer *uniformBuffer)
    {
        pipelineState->SetUniformBuffer(set, binding, uniformBuffer);
    }

    void SetTexture(uint32 set, uint32 binding, VulkanTexture *texture)
    {
        pipelineState->SetTexture(set, binding, texture);
    }

    void BindDescriptorSets(VulkanCommandBuffer &commandBuffer)
    {
        pipelineState->BindDescriptorSets(commandBuffer);
    }

    vk::raii::Pipeline &GetHandle() { return pipeline; }
    
  protected:
    std::vector<vk::raii::DescriptorSetLayout> descriptorSetLayouts;
  
    vk::raii::PipelineLayout pipelineLayout;

    vk::raii::Pipeline pipeline;

    Scope<VulkanPipelineState> pipelineState;

};

class VulkanGraphicsPipeline : public RHIGraphicsPipeline, public VulkanPipeline
{
  public:
    VulkanGraphicsPipeline(const RHIGraphicsPipelineDesc &desc, VulkanDevice &device);

    virtual void SetDynamicOffsetIndex(ShaderStage::Stage stage, const std::string &name, uint32 index) override;

    virtual void SetUniformBuffer(ShaderStage::Stage stage, const std::string &name, RHIBuffer *buffer) override;

    virtual void SetTexture(ShaderStage::Stage stage, const std::string &name, RHITexture *texture) override;

    virtual void ClearResources() override
    {
        pipelineState->ClearResources();
    }

    bool IsDirty() { return pipelineState->IsDirty(); }

  private:
    VulkanDevice &device;
};

}