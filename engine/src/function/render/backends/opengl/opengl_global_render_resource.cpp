#include "opengl_global_render_resource.h"
#include "opengl_render_resource.h"
#include "opengl_render_resource_template.h"

namespace Zafkiel
{

Scope<GlobalRenderResource> OpenGLGlobalRenderResourceFactory::Create(const Path &path)
{
    auto schema = CreateScope<RenderResourceSchema>(path);
    auto renderResourceTemplate = OpenGLRenderResourceTemplateFactory::Create(schema);
    auto renderResource = OpenGLRenderResourceFactory::Create(renderResourceTemplate);
    auto result = CreateScope<GlobalRenderResource>(std::move(schema), std::move(renderResourceTemplate), std::move(renderResource));
    return result;
}
  
}