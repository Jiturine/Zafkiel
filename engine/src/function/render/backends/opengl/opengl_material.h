#pragma once
#include "function/render/material.h"
#include "opengl_render_resource.h"

namespace Zafkiel 
{

class OpenGLMaterialFactory
{
  public:
    static Scope<Material> Create(const MaterialSpecification &spec);
};

}