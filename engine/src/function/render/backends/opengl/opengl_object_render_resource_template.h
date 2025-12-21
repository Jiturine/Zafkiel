#pragma once
#include "function/render/object_render_resource_template.h"

namespace Zafkiel 
{

class OpenGLObjectRenderResourceTemplateBackend final : public ObjectRenderResourceTemplateBackend
{
  public:
    OpenGLObjectRenderResourceTemplateBackend() = default;
    void InitTemplate(Observer<ObjectRenderResourceTemplate> renderResourceTemplate) {}
};

class OpenGLObjectRenderResourceTemplateFactory
{
  public:
    static Scope<ObjectRenderResourceTemplate> Create(const Path &path);
};

}