#pragma once
#include "Function/RHI/RHIResources.h"

namespace Zafkiel
{

class PresentPass
{
  public:
    PresentPass(Ref<RHIViewport> viewport);

    void Render(RHITexture *texture);

  private:
    Ref<RHIBuffer> vertexBuffer;
    Ref<RHIBuffer> indexBuffer;

    Ref<RHIVertexShader> vertexShader;
    Ref<RHIFragmentShader> fragmentShader;

    Ref<RHIGraphicsPipeline> pipeline;

    Ref<RHIViewport> viewport;
    
};

}