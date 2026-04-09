#pragma once
#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include "Function/RHI/RHI.h"
#include "Function/Window/Window.h"
#include "Function/RHI/Backends/OpenGL/OpenGLCommandList.h"
#include "Function/RHI/Backends/OpenGL/OpenGLFrameBuffer.h"

namespace Zafkiel
{

class OpenGLRHI final : public RHI
{
  public:
    OpenGLRHI(Window &window);

    ~OpenGLRHI();

    virtual RHIGraphicsContext *GetGraphicsContext() override { return graphicsContext.get(); }

    virtual GraphicsAPI GetAPI() const override { return GraphicsAPI::OpenGL; }
  
    virtual Ref<RHIBuffer> CreateBuffer(RHICommandList &RHICmdList, const RHIBufferDesc &desc, const void *data = nullptr) override;
  
    virtual Ref<RHITexture> CreateTexture(RHICommandList &RHICmdList, const RHITextureDesc &desc, Buffer data = nullptr) override;

    virtual Ref<RHIGraphicsPipeline> CreateGraphicsPipeline(const RHIGraphicsPipelineDesc &desc) override;

    virtual Ref<RHIVertexShader> CreateVertexShader(const Path &path) override;

    virtual Ref<RHIFragmentShader> CreateFragmentShader(const Path &path) override;

    virtual Ref<DynamicUniformBufferContent> CreateDynamicUniformBufferContent(uint32 maxSize, const ShaderReflection::UniformBlock *uniformBlock) override;
  
    virtual void WaitIdle() override {}

    const SDL_GLContext &GetGLContext() const { return glContext; }

    SDL_Window *GetWindow() { return window; }

    OpenGLFrameBufferManager &GetFrameBufferManager() { return *frameBufferManager.get(); }

    virtual ImTextureRef RegisterImGuiTexture(RHITexture *texture) override;

    virtual void UnregisterImGuiTexture(RHITexture *texture) override;

  private:
    SDL_Window *window;
  
    SDL_GLContext glContext;

    Scope<OpenGLGraphicsContext> graphicsContext; 

    Scope<OpenGLFrameBufferManager> frameBufferManager;
};

}
