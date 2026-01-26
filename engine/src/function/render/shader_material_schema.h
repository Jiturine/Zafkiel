#pragma once

#include "function/render/shader_compiler/glsl_schema_parser.h"
#include "platform/filesystem/filesystem.h"

namespace Zafkiel
{

struct UniformValue
{
    const ShaderReflection::UniformBlock *blockType;
    const ShaderReflection::DataType *type;
    ShaderReflection::UniformValuePath path;
    ScopedBuffer value;
};

// 纯 CPU 端
class ShaderMaterialSchema final
{
  public:
    ShaderMaterialSchema(const Path &path);
    
    const std::vector<std::optional<ResourceTypeInfo>> &GetResourceTypeInfos() const { return resourceTypeInfos; }

    bool HasAlias(const std::string &alias) const { return aliases.contains(alias); }

    std::string GetAliasFullPath(const std::string &alias) const { return aliases.at(alias); }

    const std::unordered_map<std::string, std::string> &GetAliases() const { return aliases; }

    ShaderReflection::ResourceTypeCategory GetResourceTypeCategory(const std::string &name) const 
    {
        auto binding = resourceNameToBinding.at(name);
        return resourceTypeInfos.at(binding).value().type->GetCategory();
    }

    uint32_t GetResourceBinding(const std::string &name) const 
    {
        return resourceNameToBinding.at(name);
    }
    
    const ShaderReflection::DataType *GetUniformParameterType(const std::string &path) const;

  private:
    std::vector<std::optional<ResourceTypeInfo>> resourceTypeInfos;
    std::unordered_map<std::string, uint32_t> resourceNameToBinding;
    std::unordered_map<std::string, Scope<ShaderReflection::DataType>> customDataTypes;
    std::unordered_map<std::string, Scope<ShaderReflection::ResourceType>> customResourceTypes;
    std::unordered_map<std::string, std::string> aliases;
};

}