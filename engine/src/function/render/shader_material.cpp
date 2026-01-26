#include "function/render/shader_material.h"
#include <glslang/Include/Types.h>
#include <glslang/Public/ShaderLang.h>
#include <glslang/Public/ResourceLimits.h>
#include <glslang/MachineIndependent/localintermediate.h>
#include "platform/filesystem/filesystem.h"

namespace Zafkiel
{
    
ShaderMaterial::ShaderMaterial(RenderHandle shaderMaterialSchema, std::vector<std::optional<ShaderResource>> resources, Scope<ShaderMaterialBackend> backend)
    : shaderMaterialSchema(shaderMaterialSchema), resources(std::move(resources)), backend(std::move(backend))
{
}

void ShaderMaterial::SetTexture2D(uint32_t binding, RenderHandle texture2D)
{
    resources[binding] = ShaderResource(ShaderReflection::SamplerType::Texture2D, texture2D);
    backend->SetTexture2D(binding, texture2D);
}

}