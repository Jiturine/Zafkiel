#pragma once
#include "function/render/renderer.h"
#include "function/render/render_registry.h"

namespace Zafkiel
{
class RenderCommand
{
  public:
    virtual ~RenderCommand() = default;

    RenderCommand(RenderRegistry &renderRegistry) : renderRegistry(renderRegistry) {}

    virtual void BeginRenderPass(const RenderPassBeginInfo &beginInfo) = 0;
    virtual void EndRenderPass() = 0;
    virtual void BindGraphicsPipeline(RenderHandle pipeline) = 0;
    virtual void BindGlobalMaterial(RenderHandle globalMaterial) = 0;
    virtual void BindPassMaterial(RenderHandle passMaterial) = 0;
    virtual void BindSurfaceMaterial(RenderHandle material) = 0;
    virtual void BindObjectShaderMaterial(uint32_t index, RenderHandle objectShaderMaterial) = 0;
    virtual void DrawIndexed(RenderHandle vertexBuffer, RenderHandle indexBuffer) = 0;
    virtual void BeginFrame() = 0;

    static Scope<RenderCommand> &Instance() { return instance; }
    friend class Renderer;

  protected:
    static Scope<RenderCommand> instance;

    RenderRegistry &renderRegistry;
};
}
