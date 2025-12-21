#pragma once
#include "function/render/render_resource_template.h"
#include "render_resource.h"
#include "shader.h"
#include "shader_family.h"
#include "texture.h"

namespace Zafkiel
{

struct MaterialSpecification
{
    std::string name;
    ShaderFamily shaderFamily;
    const Observer<RenderResourceTemplate> materialTemplate;
};

class Material final
{
  public:
    Material(const MaterialSpecification &spec, Scope<RenderResource> renderResource)
        : name(spec.name), shaderFamily(spec.shaderFamily), renderResource(std::move(renderResource)) {}

    std::string GetName() const { return name; }
    Observer<RenderResource> GetRenderResource() { return renderResource; }
    const Observer<RenderResource> GetRenderResource() const { return renderResource; }

  private:
    std::string name;
    ShaderFamily shaderFamily;
    Scope<RenderResource> renderResource;
};

}
