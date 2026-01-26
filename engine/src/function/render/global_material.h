#pragma once
#include "function/render/shader_material.h"

namespace Zafkiel 
{

class GlobalMaterial
{
  public:
    GlobalMaterial(RenderHandle schema, RenderHandle shaderMaterialTemplate, RenderHandle shaderMaterial)
        : schema(schema), shaderMaterialTemplate(shaderMaterialTemplate), shaderMaterial(shaderMaterial) {}
  
    RenderHandle GetShaderMaterial() const { return shaderMaterial; }
    RenderHandle GetTemplate() const { return shaderMaterialTemplate; }

  private:
    RenderHandle schema;
    RenderHandle shaderMaterialTemplate;
    RenderHandle shaderMaterial;
};

}