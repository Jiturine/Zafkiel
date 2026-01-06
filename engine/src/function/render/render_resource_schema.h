#pragma once

#include "glslang/Include/Types.h"
#include "glslang/Include/intermediate.h"
#include "glslang/MachineIndependent/SymbolTable.h"
#include "shader_reflection.h"
#include "platform/filesystem/filesystem.h"

namespace Zafkiel
{

struct RenderResourceParameterPath 
{
    enum PathElemType { Indent, Index };
    struct PathElem 
    {
        PathElemType type;
        std::string name;
        uint32_t index;
    };
    std::vector<PathElem> elems;
};

struct RenderResourceDescription
{
    const ShaderReflection::ResourceType *type;
    uint32_t set;
    uint32_t binding;
};

struct UniformValue
{
    const ShaderReflection::UniformBlock *blockType;
    const ShaderReflection::DataType *type;
    ShaderReflection::UniformValuePath path;
    ScopedBuffer value;
};

// 纯 CPU 端
class RenderResourceSchema final
{
  public:
    RenderResourceSchema(const Path &path);
    
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

    ShaderReflection::ResourceTypeCategory GetParameterTypeCategory(const std::string &name) const 
    {
        return parameterTypes.at(name)->GetCategory();
    }
    
    UniformValue CreateStructValue(const std::string &alias) const 
    {
        auto path = GetAliasPath(alias);
        auto uniformBlock = parameterTypes.at(path.elems[0].name)->As<ShaderReflection::UniformBlock>();
        auto valueType = GetUniformParameterType(path);
        ShaderReflection::UniformValuePath valuePath;
        for (int i = 1; i < path.elems.size(); i++)
        {
            valuePath.elems.push_back({
                path.elems[i].type == RenderResourceParameterPath::PathElemType::Indent ? ShaderReflection::UniformValuePath::PathElemType::Indent : ShaderReflection::UniformValuePath::Index,
                path.elems[i].name, 
                path.elems[i].index
            });
        }
        auto size = uniformBlock->GetStructSize(valuePath);
        return UniformValue {uniformBlock, valueType, valuePath, ScopedBuffer(size)};
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
};

}