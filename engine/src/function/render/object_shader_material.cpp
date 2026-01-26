#include "function/render/object_shader_material.h"

namespace Zafkiel 
{

ObjectShaderMaterial::ObjectShaderMaterial(RenderHandle shaderMaterialSchema, RenderHandle objectShaderMaterialTemplate, std::vector<std::optional<ObjectShaderResource>> resources, Scope<ObjectShaderMaterialBackend> backend)
    : shaderMaterialSchema(shaderMaterialSchema), objectShaderMaterialTemplate(objectShaderMaterialTemplate), resources(std::move(resources)), backend(std::move(backend))
{
    
}
    
}