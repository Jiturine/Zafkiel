#pragma once
#include "editor/function/render/editor_camera.h"
#include "function/render/frame_data.h"
#include "function/render/graphics_context.h"

namespace Zafkiel
{

class ShadingPass 
{
  public:
    ShadingPass(Observer<GlobalRenderResource> globalRenderResource, Observer<Texture2D> positionTexture, Observer<Texture2D> normalTexture, Observer<Texture2D> albedoTexture);

    Scope<GraphicsShader> shader;
    Scope<RenderPassResource> renderPassResource;
    Scope<GraphicsPipeline> pipeline;
    Scope<RenderPass> renderPass;

    Scope<VertexBuffer> vertexBuffer;
    Scope<IndexBuffer> indexBuffer;

    Observer<Texture2D> positionTexture;
    Observer<Texture2D> normalTexture;
    Observer<Texture2D> albedoTexture;

    Scope<Texture2D> outputColorTexture;

    Scope<FrameBuffer> frameBuffer;

    void Render(const FrameData &frameData);
    void Resize(uint32_t width, uint32_t height);
};
    
}