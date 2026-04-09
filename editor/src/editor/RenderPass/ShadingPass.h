#pragma once
#include "editor/Function/Render/EditorCamera.h"
#include "Function/RHI/RHIResources.h"

namespace Zafkiel
{
class GeometryPass;
class ShadowPass;

class ShadingPass 
{
  public:
    ShadingPass(GeometryPass *geometryPass, ShadowPass *shadowPass);

    Ref<RHIGraphicsPipeline> pipeline;

    Ref<RHIVertexShader> vertexShader;
    Ref<RHIFragmentShader> fragmentShader;

    Ref<RHIBuffer> vertexBuffer;
    Ref<RHIBuffer> indexBuffer;
  
    Ref<RHITexture> outputColorTexture;

    GeometryPass *geometryPass;
    ShadowPass *shadowPass;

    void Render();
    void Resize(uint32 width, uint32 height);
};
    
}