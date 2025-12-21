#include "opengl_render_pass.h"

namespace Zafkiel
{

OpenGLRenderPassBackend::OpenGLRenderPassBackend(const RenderPassSpecification &spec)
{
    
}

Scope<RenderPass> OpenGLRenderPassFactory::Create(const RenderPassSpecification &spec)
{
    auto backend = CreateScope<OpenGLRenderPassBackend>(spec);
    return CreateScope<RenderPass>(spec, std::move(backend));
}
}
