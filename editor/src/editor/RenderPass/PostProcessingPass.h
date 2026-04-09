#pragma once
#include "Function/RHI/RHIResources.h"

namespace Zafkiel 
{
class ShadingPass;

class PostProcessingPass 
{
  public:
    PostProcessingPass(ShadingPass *shadingPass);

    void Render();
    void Resize(uint32 width, uint32 height);

    Ref<RHIGraphicsPipeline> pipeline;
    
    Ref<RHITexture> outputColorTexture;

    Ref<RHIVertexShader> vertexShader;

    Ref<RHIFragmentShader> fragmentShader;

    Ref<RHIBuffer> vertexBuffer;
    Ref<RHIBuffer> indexBuffer;

    ShadingPass *shadingPass;
};

}