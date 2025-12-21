#include "opengl_render_resource_template.h"

namespace Zafkiel 
{

Scope<RenderResourceTemplate> OpenGLRenderResourceTemplateFactory::Create(const Observer<RenderResourceSchema> schema)
{
    auto backend = CreateScope<OpenGLRenderResourceTemplateBackend>();
    auto res = CreateScope<RenderResourceTemplate>(schema, std::move(backend));
    res->GetBackend().As<OpenGLRenderResourceTemplateBackend>()->InitTemplate(res);
    return res;
}

}