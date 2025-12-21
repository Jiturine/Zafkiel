#pragma once
#include "renderer.h"

namespace Zafkiel
{
class RenderCommand
{
  public:
    virtual ~RenderCommand() = default;
    static void BeginRenderPass(const RenderPassBeginInfo &beginInfo) { instance->BeginRenderPassImpl(beginInfo); }
    static void EndRenderPass() { instance->EndRenderPassImpl(); }
    static void BindPipeline(const Observer<Pipeline> pipeline) { instance->BindPipelineImpl(pipeline); }
    static void BindGlobalRenderResource(const Observer<GlobalRenderResource> globalRenderResource) { instance->BindGlobalRenderResourceImpl(globalRenderResource); }
    static void BindRenderPassResource(const Observer<RenderPassResource> renderPassResource) { instance->BindRenderPassResourceImpl(renderPassResource); }
    static void BindMaterial(const Observer<Material> material) { instance->BindMaterialImpl(material); }
    static void BindObjectRenderResource(uint32_t index, const Observer<ObjectRenderResource> objectRenderResource) { instance->BindObjectRenderResourceImpl(index, objectRenderResource); }
    static void DrawIndexed(const Observer<VertexBuffer> vertexBuffer, const Observer<IndexBuffer> indexBuffer) { instance->DrawIndexedImpl(vertexBuffer, indexBuffer); }
    static Scope<RenderCommand> &GetHandle() { return instance; }
    friend class Renderer;
  private:
    virtual void BeginRenderPassImpl(const RenderPassBeginInfo &beginInfo) = 0;
    virtual void EndRenderPassImpl() = 0;
    virtual void BindPipelineImpl(const Observer<Pipeline> pipeline) = 0;
    virtual void BindGlobalRenderResourceImpl(const Observer<GlobalRenderResource> globalRenderResource) = 0;
    virtual void BindRenderPassResourceImpl(const Observer<RenderPassResource> renderPassResource) = 0;
    virtual void BindMaterialImpl(const Observer<Material> material) = 0;
    virtual void BindObjectRenderResourceImpl(uint32_t index, const Observer<ObjectRenderResource> objectRenderResource) = 0;
    virtual void DrawIndexedImpl(const Observer<VertexBuffer> vertexBuffer, const Observer<IndexBuffer> indexBuffer) = 0;
    virtual void BeginFrame(const Scope<GraphicsContext> &context) = 0;
    inline static Scope<RenderCommand> instance = nullptr;
};
}
