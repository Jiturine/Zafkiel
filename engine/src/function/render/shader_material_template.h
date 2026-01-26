#pragma once
#include "function/render/render_handle.h"
#include "function/render/shader_material_schema.h"

namespace Zafkiel
{
class ShaderMaterialTemplateBackend
{
  public:
    virtual ~ShaderMaterialTemplateBackend() = default;
};

class ShaderMaterialTemplate final
{
  public:
    ShaderMaterialTemplate(RenderHandle schema, Scope<ShaderMaterialTemplateBackend> backend)
        : schema(schema), backend(std::move(backend)) {}

    Borrow<ShaderMaterialTemplateBackend> GetBackend() const { return Borrow(backend); }
    MutBorrow<ShaderMaterialTemplateBackend> GetBackend() { return MutBorrow(backend); }

    RenderHandle GetSchema() const { return schema; }

  private:
    RenderHandle schema;
    Scope<ShaderMaterialTemplateBackend> backend;
};

}