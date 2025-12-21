#include "render_resource.h"
#include <glslang/Include/Types.h>
#include <glslang/Public/ShaderLang.h>
#include "glslang/Public/ResourceLimits.h"
#include "glslang/MachineIndependent/localintermediate.h"
#include "platform/filesystem/filesystem.h"

namespace Zafkiel
{
    
RenderResource::RenderResource(const Observer<RenderResourceTemplate> renderResourceTemplate, Scope<RenderResourceBackend> backend)
    : renderResourceTemplate(renderResourceTemplate), backend(std::move(backend))
{
    for (auto &[paramName, paramType] : renderResourceTemplate->GetSchema()->GetParameterTypes())
    {
        if (paramType->GetCategory() == ShaderReflection::ResourceTypeCategory::UniformBlock)
        {
            parameters[paramName] = RenderResourceParameter(paramType->As<ShaderReflection::UniformBlock>());
            parameterBindings[paramName] = renderResourceTemplate->GetSchema()->GetParameterBinding(paramName);
        }
        else if (paramType->GetCategory() == ShaderReflection::ResourceTypeCategory::SampledImage)
        {
            parameters[paramName] = RenderResourceParameter(paramType->As<ShaderReflection::SampledImage>()->GetSamplerType()); // TODO: default val
            parameterBindings[paramName] = renderResourceTemplate->GetSchema()->GetParameterBinding(paramName);
        }
    }
}

Observer<Texture2D> RenderResource::GetTexture2D(const std::string &key) const 
{
    auto paramPath = renderResourceTemplate->GetSchema()->GetAliasPath(key);
    if (paramPath.elems.size() == 1 && paramPath.elems[0].type == RenderResourceParameterPath::PathElemType::Indent)
    {
        return parameters.at(paramPath.elems[0].name).texture2D; 
    }
    Log::Error("Texture2D Path doesn't exist!");
    return nullptr;
}

void RenderResource::SetTexture2D(const std::string &key, Observer<Texture2D> tex)
{
    auto paramPath = renderResourceTemplate->GetSchema()->GetAliasPath(key);
    if (paramPath.elems.size() == 1 && paramPath.elems[0].type == RenderResourceParameterPath::PathElemType::Indent)
    {
        parameters[paramPath.elems[0].name] = tex;
    }
    backend->SetTexture2D(paramPath.elems[0].name, tex);
}
}