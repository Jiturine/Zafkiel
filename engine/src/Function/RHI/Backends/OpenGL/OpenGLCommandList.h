#pragma once
#include "Function/RHI/RHICommandList.h"

namespace Zafkiel
{

class OpenGLRHI;
class OpenGLFrameBuffer;
class OpenGLGraphicsPipeline;

class OpenGLGraphicsContext : public RHIGraphicsContext
{
  public:
    OpenGLGraphicsContext(OpenGLRHI &rhi) : rhi(rhi) {}

    virtual void WriteBuffer(RHIBuffer *buffer, Buffer data) override;

    virtual void UpdateUniformBuffer(RHIBuffer *uniformBuffer, Buffer data) override;

    virtual void BeginRenderPass(const RHIRenderPassInfo &renderPassInfo) override;

    virtual void EndRenderPass() override;

    virtual void BindGraphicsPipeline(RHIGraphicsPipeline *pipeline) override;
    
    virtual void DrawIndexed(RHIBuffer *vertexBuffer, RHIBuffer *indexBuffer, uint32 indexCount = 0) override;
  
    virtual void SetStaticUniformBuffer(const std::string &name, RHIBuffer *uniformBuffer) override;

    virtual void Present() override;

    virtual void Finalize() override {}

    virtual void Submit() override {}
  
    virtual void Resize(uint32 width, uint32 height) override {}

  private:
    void ApplyStaticUniformBuffers(RHIShader *shader);

    OpenGLRHI &rhi;

    OpenGLFrameBuffer *currentFrameBuffer;

    OpenGLGraphicsPipeline *currentPipeline;

    std::unordered_map<std::string, RHIBuffer *> staticUniformBuffers;
};

}