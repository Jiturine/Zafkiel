#include "Function/RHI/Backends/OpenGL/OpenGLViewport.h"
#include "Function/Render/Renderer.h"

namespace Zafkiel
{

OpenGLViewport::OpenGLViewport(OpenGLRHI &rhi, PlatformWindow *window)
    : platformWindow(window)
{
    RHITextureDesc backendTextureDesc
    {
        .width = window->GetWidth(),
        .height = window->GetHeight(),
        .format = ImageFormat::RGBA8,
        .usages = ImageUsageFlags::ColorAttachment,
        .initialLayout = ImageLayout::Undefined,
    };
    backendTexture = CreateRef<OpenGLTexture>(rhi, backendTextureDesc);
}

void OpenGLViewport::Resize(uint32 width, uint32 height) 
{
    Renderer::Instance().GetRenderTargetPool().UpdateTexture(backendTexture, width, height);
}

}