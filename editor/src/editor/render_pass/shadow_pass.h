#pragma once
#include "function/render/graphics_context.h"
#include "function/render/frame_data.h"

namespace Zafkiel 
{

class ShadowPass 
{
  public:
    ShadowPass(RenderHandle objectShaderMaterial);
    void Render(const FrameData &frameData);
    void Resize(uint32_t width, uint32_t height);

  private:
    RenderHandle shadowFrameBuffer;
    RenderHandle shadowMap;

    RenderHandle passMaterial;
    RenderHandle shader;
    RenderHandle pipeline;
    RenderHandle renderPass;
    
    RenderHandle objectShaderMaterial;
  
};

}