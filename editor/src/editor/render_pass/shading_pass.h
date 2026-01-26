#pragma once
#include "editor/function/render/editor_camera.h"
#include "function/render/frame_data.h"
#include "function/render/graphics_context.h"

namespace Zafkiel
{

class ShadingPass 
{
  public:
    ShadingPass(RenderHandle globalMaterial, RenderHandle positionTexture, RenderHandle normalTexture, RenderHandle albedoTexture);

    RenderHandle shader;
    RenderHandle passMaterial;
    RenderHandle pipeline;
    RenderHandle renderPass;

    RenderHandle vertexBuffer;
    RenderHandle indexBuffer;

    RenderHandle positionTexture;
    RenderHandle normalTexture;
    RenderHandle albedoTexture;

    RenderHandle outputColorTexture;

    RenderHandle frameBuffer;
    
    RenderHandle globalMaterial;

    void Render(const FrameData &frameData);
    void Resize(uint32_t width, uint32_t height);
};
    
}