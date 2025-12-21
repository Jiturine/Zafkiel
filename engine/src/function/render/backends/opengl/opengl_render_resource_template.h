#pragma once
#include "function/render/render_resource_template.h"

namespace Zafkiel 
{

class OpenGLRenderResourceTemplateBackend final : public RenderResourceTemplateBackend
{
  public:
    OpenGLRenderResourceTemplateBackend() = default;
    void InitTemplate(Observer<RenderResourceTemplate> renderResourceTemplate) {}
};

class OpenGLRenderResourceTemplateFactory
{
  public:
    static Scope<RenderResourceTemplate> Create(const Observer<RenderResourceSchema> schema);
};

}