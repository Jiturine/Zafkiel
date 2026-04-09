#pragma once
#include "Function/RHI/RHIResources.h"

namespace Zafkiel
{
class RHI;

class RHICommandContext
{
  public:
    virtual PipelineType GetPipelineType() const = 0;
};

class RHIGraphicsContext : public RHICommandContext
{
  public:
    virtual PipelineType GetPipelineType() const override
    {
        return PipelineType::Graphics;
    }  

    virtual void WriteBuffer(RHIBuffer *buffer, Buffer data) = 0;
    
    virtual void UpdateUniformBuffer(RHIBuffer *uniformBuffer, Buffer data) = 0;
    
    virtual void BeginRenderPass(const RHIRenderPassInfo &renderPassInfo) = 0;

    virtual void EndRenderPass() = 0;

    virtual void BindGraphicsPipeline(RHIGraphicsPipeline *pipeline) = 0;

    virtual void DrawIndexed(RHIBuffer *vertexBuffer, RHIBuffer *indexBuffer, uint32 indexCount = 0) = 0;

    virtual void SetStaticUniformBuffer(const std::string &name, RHIBuffer *uniformBuffer) = 0;

    virtual void Present() = 0;

    virtual void Finalize() = 0;

    virtual void Submit() = 0;

    virtual void Resize(uint32 width, uint32 height) = 0;
};

class RHICommandList
{
  public:
    RHICommandList(RHIGraphicsContext *graphicsContext, RHI &rhi)
        : graphicsContext(graphicsContext), rhi(rhi) {}

    template <typename T>
    void EnqueueLambda(T &&lambda)
    {
        if (IsImmediate())
        {
            lambda(*this);
        }
        else 
        {
            list.push_back(std::forward<T>(lambda));
        }
    }
    
    virtual bool IsImmediate() const = 0;
    
    virtual void SubmitAndWaitIdle() = 0;
    
    virtual void Submit() = 0;

    RHIGraphicsContext *GetGraphicsContext()
    {
        return graphicsContext;
    }
    
  protected:
    std::vector<std::function<void(RHICommandList &)>> list;
  
    RHIGraphicsContext *graphicsContext;
  
    // RHIComputeContext *computeContext;
    
    RHI &rhi;
};

class RHICommandListImmediate : public RHICommandList
{
  public:
    RHICommandListImmediate(RHIGraphicsContext *graphicsContext, RHI &rhi)
        : RHICommandList(graphicsContext, rhi) {}

    virtual bool IsImmediate() const override { return true; }
  
    virtual void Submit() override;

    virtual void SubmitAndWaitIdle() override;
  
    Ref<RHIBuffer> CreateBuffer(const RHIBufferDesc &desc, const void *data = nullptr);

    Ref<RHITexture> CreateTexture(const RHITextureDesc &desc, Buffer data = nullptr);
    
    void WriteBuffer(RHIBuffer *buffer, Buffer data);
    
    void UpdateUniformBuffer(RHIBuffer *uniformBuffer, Buffer data);
  
    void BeginRenderPass(const RHIRenderPassInfo &renderPassInfo);

    void EndRenderPass();
  
    void BindGraphicsPipeline(RHIGraphicsPipeline *pipeline);
  
    void DrawIndexed(RHIBuffer *vertexBuffer, RHIBuffer *indexBuffer, uint32 indexCount = 0);

    void SetStaticUniformBuffer(const std::string &name, RHIBuffer *uniformBuffer);

    void Present();

    void FinalizeContext();
};

inline Scope<RHICommandListImmediate> GlobalRHICmdList;

}