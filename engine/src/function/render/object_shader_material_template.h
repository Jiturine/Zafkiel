#pragma once
#include "function/render/render_handle.h"
#include "function/render/shader_material_template.h"

namespace Zafkiel
    {
class ObjectShaderMaterialTemplateBackend
{
  public:
    virtual ~ObjectShaderMaterialTemplateBackend() = default;
};

struct ObjectShaderMaterialDescription
{
    const ShaderReflection::ResourceType *type;
    uint32_t set;
    uint32_t binding;
};

class ObjectShaderMaterialTemplate final
{
  public:
    ObjectShaderMaterialTemplate(RenderHandle schema, Scope<ObjectShaderMaterialTemplateBackend> backend)
        : schema(schema), backend(std::move(backend)) {}
    
    Borrow<ObjectShaderMaterialTemplateBackend> GetBackend() const { return Borrow(backend); }
    MutBorrow<ObjectShaderMaterialTemplateBackend> GetBackend() { return MutBorrow(backend); }

    RenderHandle GetSchema() const { return schema; }

  private:
    RenderHandle schema;
    Scope<ObjectShaderMaterialTemplateBackend> backend;
};

}
