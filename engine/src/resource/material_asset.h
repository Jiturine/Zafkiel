#pragma once
#include "core/base/memory.h"
#include "function/render/material.h"
#include "resource/asset.h"
#include "resource/asset_manager.h"
#include "resource/texture2d_asset.h"

namespace Zafkiel
{

struct MaterialAssetParameter
{
    const ShaderReflection::ResourceType *type;
    ScopedBuffer uniformBuffer;
    AssetHandle handle;
    
    MaterialAssetParameter() = default;
    MaterialAssetParameter(const ShaderReflection::UniformBlock *uniformBlock) : type(uniformBlock), uniformBuffer(ScopedBuffer(uniformBlock->GetLayout().size)) {}
    MaterialAssetParameter(ShaderReflection::SamplerType samplerType, AssetHandle handle) : type(ShaderReflection::GetSampledImageType(samplerType)), uniformBuffer(nullptr), handle(handle) {}
};

class MaterialAssetData final 
{
  public:
    MaterialAssetData(const Observer<RenderResourceTemplate> renderResourceTemplate)
        : renderResourceTemplate(renderResourceTemplate) 
    {
    }

    std::unordered_map<std::string, MaterialAssetParameter> parameters;


  private:
    const Observer<RenderResourceTemplate> renderResourceTemplate;
};

class MaterialAsset : public Asset
{
  public:
    virtual AssetType GetAssetType() const override { return AssetType::Material; }

    MaterialAsset(AssetHandle handle, const std::string &name, const std::string &data);
    MaterialAsset(AssetHandle handle, const std::string &name, ShaderFamily shaderFamily);

    std::string GetName() const { return name; }
    ShaderFamily GetShaderFamily() const { return shaderFamily; }
    const std::unordered_map<std::string, MaterialAssetParameter> &GetParameters() const { return parameters; }
    
    bool HasParam(const std::string &paramAlias);
    bool HasSampledImage(const ShaderReflection::SampledImage *imageType, const std::string &paramAlias);
    
    std::string Serialize();
    
    void DeserializeParamAlias(const std::string &paramAlias, const IDeserializer &param);
    void DeserializeSampledImage(const IDeserializer &param, const ShaderReflection::SampledImage *imageType, const std::string &imageName);
    void DeserializeUniformParam(const IDeserializer &param, const ShaderReflection::DataType *paramType, RenderResourceParameterPath paramPath);
    void DeserializeStructParam(const IDeserializer &param, const ShaderReflection::Struct *structType, RenderResourceParameterPath paramPath);
    void DeserializeFundamentalParam(const IDeserializer &param, const ShaderReflection::Fundamental *fundamentalType, RenderResourceParameterPath paramPath);
    void DeserializeArrayParam(const IDeserializer &param, const ShaderReflection::Array *arrayType, RenderResourceParameterPath paramPath);

    template <typename T>
    void SetUniform(const std::string &key, T value)
    {
        auto path = schema->GetAliasPath(key);
        SetUniform(path, value);
    }

    template <typename T>
    void SetUniform(const RenderResourceParameterPath &path, T value);
    
    template <typename T>
    T GetUniform(const std::string &key)
    {
        auto paramPath = schema->GetAliasPath(key);
        return GetUniform<T>(paramPath);
    }

    template <typename T>
    T GetUniform(const RenderResourceParameterPath &path);

    void SetTexture2D(const std::string &key, AssetHandle handle)
    {
        auto paramPath = schema->GetAliasPath(key);
        if (paramPath.elems.size() == 1 && paramPath.elems[0].type == RenderResourceParameterPath::PathElemType::Indent)
        {
            parameters[paramPath.elems[0].name] = {ShaderReflection::SamplerType::Texture2D, handle};
        }
    }

    AssetHandle GetTexture2D(const std::string &key) const 
    {
        auto paramPath = schema->GetAliasPath(key);
        if (paramPath.elems.size() == 1 && paramPath.elems[0].type == RenderResourceParameterPath::PathElemType::Indent)
        {
            return parameters.at(paramPath.elems[0].name).handle; 
        }
        Log::Error("Texture2D Path doesn't exist!");
        return {};
    }
    
    bool HasTexture2D(const std::string &key) const 
    {
        
        auto paramPath = schema->GetAliasPath(key);
        if (paramPath.elems.size() == 1 && paramPath.elems[0].type == RenderResourceParameterPath::PathElemType::Indent)
        {
            return parameters.contains(paramPath.elems[0].name);
        }
        return false;
    }
    
    void SerializeParam(const std::string &paramAlias, ISerializer &serializer);
    void SerializeStructParam(ISerializer &serializer, const ShaderReflection::Struct *structType, RenderResourceParameterPath paramPath);
    void SerializeFundamentalParam(ISerializer &serializer, const ShaderReflection::Fundamental *fundamentalType, RenderResourceParameterPath paramPath);
    void SerializeArrayParam(ISerializer &serializer, const ShaderReflection::Array *arrayType, RenderResourceParameterPath paramPath);
    void SerializeUniformParam(ISerializer &serializer, const ShaderReflection::DataType *type, RenderResourceParameterPath paramPath);
    void SerializeSampledImage(ISerializer &serializer, const ShaderReflection::SampledImage *imageType, const std::string &paramAlias);

  private:
    Observer<RenderResourceSchema> schema;
    std::unordered_map<std::string, MaterialAssetParameter> parameters;
    std::string name;
    ShaderFamily shaderFamily;
};

}

#include "material_asset.tpp"