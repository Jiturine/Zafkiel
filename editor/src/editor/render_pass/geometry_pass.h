#pragma once
#include "function/render/frame_data.h"
#include "function/render/graphics_context.h"
#include "function/render/material.h"
#include "editor/function/render/editor_camera.h"

namespace Zafkiel
{

class GeometryPass
{
  public:
    GeometryPass(Observer<GlobalRenderResource> globalRenderResource, Observer<ObjectRenderResource> objectRenderResource);

    Scope<GraphicsShader> blinnPhongShader;
    Scope<GraphicsPipeline> blinnPhongPipeline;
    Scope<RenderPass> renderPass;
    Observer<ObjectRenderResource> objectRenderResource;

    Scope<Texture2D> positionTexture;
    Scope<Texture2D> normalTexture;
    Scope<Texture2D> albedoTexture;
    Scope<Texture2D> entityIDTexture;
    Scope<Image> depthImage;

    Scope<FrameBuffer> frameBuffer;

    void Render(const FrameData &frameData);
    void Resize(uint32_t width, uint32_t height);
};

}
