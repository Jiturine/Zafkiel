#pragma once
#include "render_resource.h"
#include "render_resource_template.h"

namespace Zafkiel 
{

class RenderPassResource 
{
  public:
    RenderPassResource(Scope<RenderResourceSchema> schema, Scope<RenderResourceTemplate> renderResourceTemplate, Scope<RenderResource> renderResource)
        : schema(std::move(schema)), renderResourceTemplate(std::move(renderResourceTemplate)), renderResource(std::move(renderResource)) {}

    Observer<RenderResource> GetRenderResource() { return renderResource; }
    const Observer<RenderResource> GetRenderResource() const { return renderResource; }

  private:
    Scope<RenderResourceSchema> schema;
    Scope<RenderResource> renderResource;
    Scope<RenderResourceTemplate> renderResourceTemplate;
};

}