#include "opengl_material.h"

namespace Zafkiel
{
    
Scope<Material> OpenGLMaterialFactory::Create(const MaterialSpecification &spec)
{
    auto renderResource = OpenGLRenderResourceFactory::Create(spec.materialTemplate);
    auto material = CreateScope<Material>(spec, std::move(renderResource));
    return material;
}
  
}