#pragma once
#include "function/render/shader_material.h"
#include "function/render/shader_material_template.h"

namespace Zafkiel 
{

class PassMaterial 
{
  public:
    PassMaterial(RenderHandle schema, RenderHandle shaderMaterialTemplate, RenderHandle shaderMaterial)
        : schema(schema), shaderMaterialTemplate(shaderMaterialTemplate), shaderMaterial(shaderMaterial) {}

    RenderHandle GetShaderMaterial() const { return shaderMaterial; }

    RenderHandle GetTemplate() const { return shaderMaterialTemplate; }

  private:
    RenderHandle schema;
    RenderHandle shaderMaterialTemplate;
    RenderHandle shaderMaterial;
};

}