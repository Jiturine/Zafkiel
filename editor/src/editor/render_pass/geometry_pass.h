#pragma once
#include "function/render/frame_data.h"
#include "function/render/graphics_context.h"
#include "function/render/surface_material.h"
#include "editor/function/render/editor_camera.h"

namespace Zafkiel
{

class GeometryPass
{
  public:
    GeometryPass(RenderHandle globalMaterial, RenderHandle objectShaderMaterial);

    RenderHandle blinnPhongShader;
    RenderHandle blinnPhongPipeline;
    RenderHandle renderPass;
    RenderHandle objectShaderMaterial;

    RenderHandle positionTexture;
    RenderHandle normalTexture;
    RenderHandle albedoTexture;
    RenderHandle entityIDTexture;
    RenderHandle depthImage;

    RenderHandle frameBuffer;

    void Render(const FrameData &frameData);
    void Resize(uint32_t width, uint32_t height);
};

}
