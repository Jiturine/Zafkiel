#pragma once
#include "function/render/render_command.h"

namespace Zafkiel
{

class OpenGLCommand : public RenderCommand
{
  public:
    OpenGLCommand() = default;
  private:
    virtual void BeginRenderPassImpl(const RenderPassBeginInfo &beginInfo) override;
    virtual void EndRenderPassImpl() override;
    virtual void BindPipelineImpl(const Observer<Pipeline> pipeline) override;
    virtual void BindGlobalRenderResourceImpl(const Observer<GlobalRenderResource> globalRenderResource) override;
    virtual void BindRenderPassResourceImpl(const Observer<RenderPassResource> renderPassResource) override;
    virtual void BindMaterialImpl(const Observer<Material> material) override;
    virtual void BindObjectRenderResourceImpl(uint32_t index, const Observer<ObjectRenderResource> objectRenderResource) override;
    virtual void DrawIndexedImpl(const Observer<VertexBuffer> vertexBuffer, const Observer<IndexBuffer> indexBuffer) override;
    virtual void BeginFrame(const Scope<GraphicsContext> &graphicsContext) override;

    Observer<FrameBuffer> currentFrameBuffer;
    Observer<Pipeline> currentPipeline;
};

}
