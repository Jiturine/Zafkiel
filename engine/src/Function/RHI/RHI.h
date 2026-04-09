#pragma once
#include "Function/RHI/RHIResources.h"
#include "Function/RHI/RHICommandList.h"
#include "Function/RHI/GraphicsAPI.h"
#include "Platform/Filesystem/Filesystem.h"

#include <imgui.h>

namespace Zafkiel
{

class RHI
{
  public:
    virtual ~RHI() = default;

    virtual GraphicsAPI GetAPI() const = 0;
  
    virtual void WaitIdle() = 0;

    virtual RHIGraphicsContext *GetGraphicsContext() = 0;

    virtual Ref<RHIBuffer> CreateBuffer(RHICommandList &RHICmdList, const RHIBufferDesc &desc, const void *data = nullptr) = 0;
  
    virtual Ref<RHITexture> CreateTexture(RHICommandList &RHICmdList, const RHITextureDesc &desc, Buffer data = nullptr) = 0;

    virtual Ref<RHIGraphicsPipeline> CreateGraphicsPipeline(const RHIGraphicsPipelineDesc &desc) = 0;

    virtual Ref<RHIVertexShader> CreateVertexShader(const Path &path) = 0;

    virtual Ref<RHIFragmentShader> CreateFragmentShader(const Path &path) = 0;

    virtual Ref<DynamicUniformBufferContent> CreateDynamicUniformBufferContent(uint32 maxSize, const ShaderReflection::UniformBlock *uniformBlock) = 0;

    virtual ImTextureRef RegisterImGuiTexture(RHITexture *texture) = 0;

    virtual void UnregisterImGuiTexture(RHITexture *texture) = 0;
  
};

inline Scope<RHI> GlobalRHI;

}
