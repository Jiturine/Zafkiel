#pragma once
#include "Function/RHI/RHIResources.h"
#include "Function/RHI/FrameData.h"

namespace Zafkiel 
{

class ShadowPass 
{
  public:
    ShadowPass();
    void Render(const FrameData &frameData);
    void Resize(uint32 width, uint32 height);

    Ref<RHITexture> shadowMap;

    Ref<RHIGraphicsPipeline> pipeline;
  
    Ref<RHIVertexShader> vertexShader;

    Ref<RHIFragmentShader> fragmentShader;

    Ref<RHIBuffer> uShadowUniformBuffer;

    Scope<UniformBufferContent> uShadowContent;
  
    mat4 lightViewProj;
};

}