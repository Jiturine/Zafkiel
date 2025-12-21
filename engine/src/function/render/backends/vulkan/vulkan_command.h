#pragma once
#include "function/render/render_command.h"

namespace Zafkiel
{

class VulkanCommand final : public RenderCommand
{
  public:
    VulkanCommand() = default;
    vk::CommandBuffer GetCommandBuffer() { return commandBuffer; }

    void ImageMemoryBarrier(Observer<Image> image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout,
        vk::AccessFlagBits srcAccess, vk::AccessFlagBits dstAccess, vk::PipelineStageFlagBits srcStage, vk::PipelineStageFlagBits dstStage);
  private:
    virtual void BeginRenderPassImpl(const RenderPassBeginInfo &beginInfo) override;
    virtual void EndRenderPassImpl() override;
    virtual void BindPipelineImpl(const Observer<Pipeline> pipeline) override;
    virtual void BindGlobalRenderResourceImpl(const Observer<GlobalRenderResource> globalRenderResource) override;
    virtual void BindRenderPassResourceImpl(const Observer<RenderPassResource> renderPassResource) override;
    virtual void BindMaterialImpl(const Observer<Material> material) override;
    virtual void BindObjectRenderResourceImpl(uint32_t index, const Observer<ObjectRenderResource> objectRenderResource) override;
    virtual void DrawIndexedImpl(const Observer<VertexBuffer> vertexBuffer, const Observer<IndexBuffer> indexBuffer) override;
    virtual void BeginFrame(const Scope<GraphicsContext> &context) override;

    void BindGlobalRenderResourceInPipeline();
    void BindRenderPassResourceInPipeline();

    vk::CommandBuffer commandBuffer;
    uint32_t curFrame;
    Observer<GlobalRenderResource> currentGlobalRenderResource;
    Observer<RenderPassResource> currentRenderPassResource;
    Observer<Pipeline> currentPipeline;
    Observer<FrameBuffer> currentFrameBuffer;
    
    bool globalRenderResourceChanged = false;
    bool renderPassResourceChanged = false;
};

}
