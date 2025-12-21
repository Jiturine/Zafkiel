#pragma once
#include "render_resource_template.h"
#include <cstdint>
#include <sys/types.h>

namespace Zafkiel
    {
class ObjectRenderResourceTemplateBackend
{
  public:
    virtual ~ObjectRenderResourceTemplateBackend() = default;
};

struct ObjectRenderResourceDescription
{
    const ShaderReflection::ResourceType *type;
    uint32_t set;
    uint32_t binding;
};

class ObjectRenderResourceTemplate final
{
  public:
    ObjectRenderResourceTemplate(const Path &path, Scope<ObjectRenderResourceTemplateBackend> backend);
    const std::unordered_map<std::string, const ShaderReflection::ResourceType *> &GetParameterTypes() const
    {
        return parameterTypes;
    }
    const uint32_t GetParameterBinding(const std::string &name) const
    {
        return bindings.at(name);
    }
    bool HasAlias(const std::string &alias) const
    {
        return aliases.contains(alias);
    }
    RenderResourceParameterPath GetAliasPath(const std::string &alias) const
    {
        return aliases.at(alias);
    }
    const std::unordered_map<std::string, RenderResourceParameterPath> &GetAliases() const { return aliases; }

    Observer<ObjectRenderResourceTemplateBackend> GetBackend() { return backend; }
    const Observer<ObjectRenderResourceTemplateBackend> GetBackend() const { return backend; }

    ShaderReflection::ResourceTypeCategory GetParameterTypeCategory(const std::string &name) const
    {
        return parameterTypes.at(name)->GetCategory();
    }

    const ShaderReflection::DataType *GetUniformParameterType(const RenderResourceParameterPath &path) const;

    RenderResourceParameterPath ParseParameterPath(const std::string &path) const;

  private:
    void TraverseAST(const TIntermNode *node, const glslang::TIntermediate *intermediate);
    void ParseAliases(const std::vector<std::string> &lines);
    void ParseShaderSchema(const std::string &source);
    const ShaderReflection::DataType *ParseStructType(const glslang::TType *structType, const glslang::TIntermediate *intermediate);
    const ShaderReflection::DataType *ParseArrayType(const glslang::TType *structType, const glslang::TIntermediate *intermediate);
    const ShaderReflection::DataType *ParseFundamentalType(const glslang::TType *fundamentalType, const glslang::TIntermediate *intermediate);
    const ShaderReflection::DataType *ParseDataType(const glslang::TType *type, const glslang::TIntermediate *intermediate);
    ShaderReflection::UniformFieldLayout CalculateUniformLayout(const std::string &name, const glslang::TType *type, const glslang::TIntermediate *intermediate, const ShaderReflection::DataType *customType);
    ShaderReflection::UniformFieldLayout CalculateArrayLayout(const std::string &name, const glslang::TType *type, const glslang::TIntermediate *intermediate, const ShaderReflection::DataType *customType);
    ShaderReflection::UniformFieldLayout CalculateStructLayout(const std::string &name, const glslang::TType *type, const glslang::TIntermediate *intermediate, const ShaderReflection::DataType *customType);
    ShaderReflection::UniformFieldLayout CalculateFundamentalLayout(const std::string &name, const glslang::TType *type, const glslang::TIntermediate *intermediate, const ShaderReflection::DataType *customType);
    RenderResourceDescription ParseResourceType(const glslang::TIntermSymbol *symbol, const glslang::TIntermediate *intermediate);

    std::unordered_map<std::string, const ShaderReflection::ResourceType *> parameterTypes;
    std::unordered_map<std::string, uint32_t> bindings;
    std::unordered_map<std::string, Scope<ShaderReflection::DataType>> customDataTypes;
    std::unordered_map<std::string, Scope<ShaderReflection::ResourceType>> customResourceTypes;
    std::unordered_map<std::string, RenderResourceParameterPath> aliases;
    Scope<ObjectRenderResourceTemplateBackend> backend;
};

}
