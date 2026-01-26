#pragma once
#include "function/render/shader_material_template.h"
#include "function/render/shader_material.h"
#include "function/render/shader.h"
#include "function/render/shader_family.h"
#include "function/render/texture.h"

namespace Zafkiel
{

struct SurfaceMaterialSpecification
{
    std::string name;
    ShaderFamily shaderFamily;
    RenderHandle shaderMaterialTemplate;
};

class MaterialAsset;

class SurfaceMaterial final
{
  public:
    SurfaceMaterial(const SurfaceMaterialSpecification &spec, RenderHandle shaderMaterialHandle)
        : name(spec.name), shaderFamily(spec.shaderFamily), shaderMaterial(shaderMaterialHandle) {}

    std::string GetName() const { return name; }

    RenderHandle GetShaderMaterial() const { return shaderMaterial; }

  private:
    std::string name;
    ShaderFamily shaderFamily;
    RenderHandle shaderMaterial;
};

}
