#pragma once
#include "Function/RHI/RHICommandList.h"
#include "Function/RHI/Backends/Vulkan/VulkanSubmission.h"

namespace Zafkiel
{

class VulkanDevice;
class VulkanFrameBuffer;
class VulkanRenderPass;
class VulkanGraphicsPipeline;
class VulkanRHI;
class VulkanTextureBase;
class VulkanViewport;

vk::PipelineStageFlags ImageLayoutToPipelineStage(ImageLayout layout);

vk::AccessFlags ImageLayoutToAccessMask(ImageLayout layout);

class VulkanCommandContext
{
  public:
    VulkanCommandContext(VulkanQueue &queue, VulkanDevice &device);

    VulkanPayload *GetPayload(VulkanPayloadPhase phase);

    void NewPayload();

    void EndPayload();

    void ImageMemoryBarrier(VulkanTextureBase *texture,
                            vk::AccessFlags srcAccessMask, vk::AccessFlags dstAccessMask,
                            vk::ImageLayout oldLayout, vk::ImageLayout newLayout,
                            vk::PipelineStageFlags srcStageMask, vk::PipelineStageFlags dstStageMask);

    VulkanCommandBuffer *GetCommandBuffer();
  
    void PrepareNewCommandBuffer(VulkanPayload *payload);

  protected:
    Scope<VulkanCommandPool> commandPool;

    VulkanQueue &queue;

    VulkanDevice &device;

    std::vector<Scope<VulkanPayload>> payloads;

    VulkanPayloadPhase currentPhase;
};

class VulkanGraphicsContext : public RHIGraphicsContext, public VulkanCommandContext
{
  public:
    VulkanGraphicsContext(VulkanQueue &queue, VulkanDevice &device)
        : VulkanCommandContext(queue, device) {}

    virtual void WriteBuffer(RHIBuffer *buffer, Buffer data) override;

    virtual void UpdateUniformBuffer(RHIBuffer *uniformBuffer, Buffer data) override;
  
    virtual void BeginRenderPass(const RHIRenderPassInfo &renderPassInfo) override;

    virtual void EndRenderPass() override;

    virtual void BindGraphicsPipeline(RHIGraphicsPipeline *pipeline) override;

    virtual void DrawIndexed(RHIBuffer *vertexBuffer, RHIBuffer *indexBuffer, uint32 indexCount = 0) override;

    virtual void SetStaticUniformBuffer(const std::string &name, RHIBuffer *uniformBuffer) override;

    virtual void Present(RHIViewport *viewport) override;

    virtual void Finalize() override;
    
    virtual void Submit() override;

  private:
    void ApplyStaticUniformBuffers(RHIShader *shader);

    std::unordered_map<std::string, RHIBuffer *> staticUniformBuffers;

    VulkanViewport *currentViewport;

    Scope<RHIRenderPassInfo> currentRenderPassInfo; 

    VulkanRenderPass *currentRenderPass;

    VulkanFrameBuffer *currentFrameBuffer;

    VulkanGraphicsPipeline *currentGraphicsPipeline;
};

}