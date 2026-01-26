#pragma once
#include <glslang/Include/Types.h>
#include <glslang/Include/intermediate.h>
#include <glslang/MachineIndependent/SymbolTable.h>
#include "function/render/shader_reflection.h"

namespace Zafkiel 
{
struct ShaderMaterialParameterPath 
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

struct ResourceTypeInfo
{
    std::string name;
    const ShaderReflection::ResourceType *type;
};

class GlslSchemaParser
{
  public:
    GlslSchemaParser(const std::string &source);
    std::vector<std::optional<ResourceTypeInfo>> GetResourceTypeInfos() { return std::move(resourceTypeInfos); }
    std::unordered_map<std::string, uint32_t> GetResourceNameToBinding() { return std::move(resourceNameToBinding); }
    std::unordered_map<std::string, Scope<ShaderReflection::DataType>> GetCustomDataTypes() { return std::move(customDataTypes); }
    std::unordered_map<std::string, Scope<ShaderReflection::ResourceType>> GetCustomResourceTypes() { return std::move(customResourceTypes); }
    std::unordered_map<std::string, std::string> GetAliases() { return std::move(aliases); }

  private:
    ShaderMaterialParameterPath ParseParameterPath(const std::string &path) const;
    void TraverseAST(const TIntermNode *node, const glslang::TIntermediate *intermediate);
    void ParseAliases(const std::vector<std::string> &lines);
    void ParseShaderSchema(const std::string &source);  
    const ShaderReflection::DataType *ParseStructType(const glslang::TType *structType, const glslang::TIntermediate *intermediate);
    const ShaderReflection::DataType *ParseArrayType(const glslang::TType *structType, const glslang::TIntermediate *intermediate);
    const ShaderReflection::DataType *ParseFundamentalType(const glslang::TType *fundamentalType, const glslang::TIntermediate *intermediate);
    const ShaderReflection::DataType *ParseDataType(const glslang::TType *type, const glslang::TIntermediate *intermediate);

    void CalculateUniformLayout(const glslang::TType *type, const glslang::TIntermediate *intermediate, const ShaderReflection::DataType *customType, ShaderReflection::UniformBlockLayout &layout);
    void CalculateArrayLayout(const glslang::TType *type, const glslang::TIntermediate *intermediate, const ShaderReflection::DataType *customType, ShaderReflection::UniformBlockLayout &layout);
    void CalculateStructLayout(const glslang::TType *type, const glslang::TIntermediate *intermediate, const ShaderReflection::DataType *customType, ShaderReflection::UniformBlockLayout &layout);
    void CalculateFundamentalLayout(const glslang::TType *type, const glslang::TIntermediate *intermediate, const ShaderReflection::DataType *customType, ShaderReflection::UniformBlockLayout &layout);
    std::pair<uint32_t, ResourceTypeInfo> ParseResourceType(const glslang::TIntermSymbol *symbol, const glslang::TIntermediate *intermediate);

    std::vector<std::optional<ResourceTypeInfo>> resourceTypeInfos;
    std::unordered_map<std::string, Scope<ShaderReflection::DataType>> customDataTypes;
    std::unordered_map<std::string, Scope<ShaderReflection::ResourceType>> customResourceTypes;
    std::unordered_map<std::string, std::string> aliases;
    std::unordered_map<std::string, uint32_t> resourceNameToBinding;
};

static ShaderMaterialParameterPath ParseParameterPath(const std::string &path)
{
    ShaderMaterialParameterPath result;
    uint32_t i = 0;

    auto readIdent = [&]() {
        uint32_t start = i;
        while (i < path.size() &&
              (isalnum(path[i]) || path[i] == '_'))
            i++;
        return path.substr(start, i - start);
    };

    while (i < path.size()) 
    {
        if (isalpha(path[i]) || path[i] == '_') {
            std::string ident = readIdent();
            result.elems.push_back({
                .type = ShaderMaterialParameterPath::PathElemType::Indent,
                .name = ident
            });
        }
        else if (path[i] == '.') {
            i++;
        }
        else if (path[i] == '[') {
            i++;
            uint32_t start = i;
            while (isdigit(path[i])) i++;
            uint32_t index = std::stoi(path.substr(start, i - start));
            result.elems.push_back({
                .type = ShaderMaterialParameterPath::PathElemType::Index,
                .index = index
            });

            if (path[i] == ']') i++;
        }
        else {
            Log::Error("Invalid character in path!");
        }
    }
    return result;
}

static std::string GetResourceName(const std::string &path)
{
    uint32_t i = 0;

    auto readIdent = [&]() {
        uint32_t start = i;
        while (i < path.size() &&
              (isalnum(path[i]) || path[i] == '_'))
            i++;
        return path.substr(start, i - start);
    };

    while (i < path.size()) 
    {
        if (isalpha(path[i]) || path[i] == '_') {
            std::string ident = readIdent();
            return ident;
        }
        else {
            Log::Error("Invalid character!");
        }
    }
    Log::Error("Invalid Path Format!");
    return {};
}

}