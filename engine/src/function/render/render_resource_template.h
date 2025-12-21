#pragma once
#include "render_resource_schema.h"

namespace Zafkiel
{
class RenderResourceTemplateBackend
{
  public:
    virtual ~RenderResourceTemplateBackend() = default;
};

class RenderResourceTemplate final
{
  public:
    RenderResourceTemplate(const Observer<RenderResourceSchema> schema, Scope<RenderResourceTemplateBackend> backend)
        : schema(schema), backend(std::move(backend)) {}

    Observer<RenderResourceTemplateBackend> GetBackend() { return backend; }
    const Observer<RenderResourceTemplateBackend> GetBackend() const { return backend; }

    Observer<RenderResourceSchema> GetSchema() { return schema; }
    const Observer<RenderResourceSchema> GetSchema() const { return schema; }

  private:
    const Observer<RenderResourceSchema> schema;
    Scope<RenderResourceTemplateBackend> backend;
};

}