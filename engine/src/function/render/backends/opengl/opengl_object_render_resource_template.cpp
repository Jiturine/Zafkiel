#include "opengl_object_render_resource_template.h"

namespace Zafkiel 
{

Scope<ObjectRenderResourceTemplate> OpenGLObjectRenderResourceTemplateFactory::Create(const Path &path)
{
    auto backend = CreateScope<OpenGLObjectRenderResourceTemplateBackend>();
    auto res = CreateScope<ObjectRenderResourceTemplate>(path, std::move(backend));
    res->GetBackend().As<OpenGLObjectRenderResourceTemplateBackend>()->InitTemplate(res);
    return res;
}

}