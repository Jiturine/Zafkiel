#pragma once
#include "Function/RHI/FrameData.h"
#include "Function/RHI/RHIResources.h"
#include "editor/Function/Render/EditorCamera.h"

namespace Zafkiel
{

class GeometryPass
{
  public:
    GeometryPass();

    Ref<RHIGraphicsPipeline> pbrPipeline;
  
    Ref<RHIVertexShader> vertexShader;
    Ref<RHIFragmentShader> fragmentShader;

    Ref<RHITexture> positionTexture;
    Ref<RHITexture> normalTexture;
    Ref<RHITexture> albedoTexture;
    Ref<RHITexture> metalnessTexture;
    Ref<RHITexture> roughnessTexture;
    Ref<RHITexture> entityIDTexture;
    Ref<RHITexture> depthTexture;

    void Render(const FrameData &frameData);
    void Resize(uint32 width, uint32 height);
};

}
