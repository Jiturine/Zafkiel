#pragma once
#include "Platform/PlatformWindow/PlatformWindow.h"
#include "Function/RHI/RHIResources.h"
#include "Function/RHI/Backends/OpenGL/OpenGLTexture.h"
#include <SDL3/SDL.h>

namespace Zafkiel
{

class OpenGLViewport : public RHIViewport
{
  public:
    OpenGLViewport(OpenGLRHI &rhi, PlatformWindow *window);

    PlatformWindow *GetPlatformWindow() { return platformWindow; }

    virtual RHITexture *GetBackendTexture() override { return backendTexture.get(); }

    virtual void Resize(uint32 width, uint32 height) override;

  private:
    Ref<RHITexture> backendTexture;

    PlatformWindow *platformWindow;
};

}