#include "Function/RHI/RHICommandList.h"
#include "Function/RHI/RHI.h"

namespace Zafkiel 
{

void RHICommandListImmediate::Submit() 
{
    graphicsContext->Submit();
}

void RHICommandListImmediate::SubmitAndWaitIdle()
{
    Submit();
    rhi.WaitIdle();
}

Ref<RHIBuffer> RHICommandListImmediate::CreateBuffer(const RHIBufferDesc &desc, const void *data)
{
    return GlobalRHI->CreateBuffer(*this, desc, data);
}

Ref<RHITexture> RHICommandListImmediate::CreateTexture(const RHITextureDesc &desc, Buffer data)
{
    return GlobalRHI->CreateTexture(*this, desc, data);
}

void RHICommandListImmediate::WriteBuffer(RHIBuffer *buffer, Buffer data)
{
    return graphicsContext->WriteBuffer(buffer, data);
}

void RHICommandListImmediate::UpdateUniformBuffer(RHIBuffer *uniformBuffer, Buffer data)
{
    graphicsContext->UpdateUniformBuffer(uniformBuffer, data);
}

void RHICommandListImmediate::BeginRenderPass(const RHIRenderPassInfo &renderPassInfo)
{
    graphicsContext->BeginRenderPass(renderPassInfo);
}

void RHICommandListImmediate::EndRenderPass()
{
    graphicsContext->EndRenderPass();
}

void RHICommandListImmediate::BindGraphicsPipeline(RHIGraphicsPipeline *pipeline)
{
    graphicsContext->BindGraphicsPipeline(pipeline);
}

void RHICommandListImmediate::DrawIndexed(RHIBuffer *vertexBuffer, RHIBuffer *indexBuffer, uint32 indexCount)
{
    graphicsContext->DrawIndexed(vertexBuffer, indexBuffer, indexCount);
}

void RHICommandListImmediate::SetStaticUniformBuffer(const std::string &name, RHIBuffer *uniformBuffer)
{
    graphicsContext->SetStaticUniformBuffer(name, uniformBuffer);
}

void RHICommandListImmediate::Present(RHIViewport *viewport)
{
    graphicsContext->Present(viewport);
}
  
void RHICommandListImmediate::FinalizeContext()
{
    graphicsContext->Finalize();
}

}