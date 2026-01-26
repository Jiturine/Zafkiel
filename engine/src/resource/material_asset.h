#pragma once
#include "core/base/memory.h"
#include "resource/asset.h"
#include "resource/asset_manager.h"
#include "resource/texture2d_asset.h"
#include "function/render/renderer.h"

namespace Zafkiel
{

struct MaterialAssetResource
{
    const ShaderReflection::ResourceType *type;
    ScopedBuffer uniformBuffer;
    AssetHandle handle;
    
    MaterialAssetResource() = default;
    MaterialAssetResource(const ShaderReflection::UniformBlock *uniformBlock) : type(uniformBlock), uniformBuffer(ScopedBuffer(uniformBlock->GetSize())) {}
    MaterialAssetResource(ShaderReflection::SamplerType samplerType, AssetHandle handle) : type(ShaderReflection::GetSampledImageType(samplerType)), uniformBuffer(nullptr), handle(handle) {}
};

class MaterialAsset : public Asset
{
  public:
    virtual AssetType GetAssetType() const override { return AssetType::Material; }

    MaterialAsset(AssetHandle handle, const std::string &name, const std::string &data);
    MaterialAsset(AssetHandle handle, const std::string &name, ShaderFamily shaderFamily);

    std::string GetName() const { return name; }
    ShaderFamily GetShaderFamily() const { return shaderFamily; }
    const std::vector<std::optional<MaterialAssetResource>> &GetResources() const { return resources; }
    std::vector<std::optional<MaterialAssetResource>> &GetResources() { return resources; }
    
    std::string Serialize();
    
    void DeserializeParam(const std::string &paramAlias, const IDeserializer &param);
    void DeserializeSampledImage(const IDeserializer &param, const ShaderReflection::SampledImage *imageType, const std::string &imageName);
    void DeserializeUniformParam(const IDeserializer &param, const ShaderReflection::DataType *paramType, std::string paramPath);
    void DeserializeStructParam(const IDeserializer &param, const ShaderReflection::Struct *structType, std::string paramPath);
    void DeserializeFundamentalParam(const IDeserializer &param, const ShaderReflection::Fundamental *fundamentalType, std::string paramPath);
    void DeserializeArrayParam(const IDeserializer &param, const ShaderReflection::Array *arrayType, std::string paramPath);

    template <typename T>
    void SetUniform(const std::string &alias, T value)
    {
        auto schemaHandle = Renderer::Instance().GetBuiltInMaterialSchema(shaderFamily);
        auto schema = Renderer::Instance().GetShaderMaterialSchema(schemaHandle);
        auto path = schema->GetAliasFullPath(alias);
        SetUniformByPath(path, value);
    }

    template <typename T>
    void SetUniformByPath(const std::string &path, T value);
    
    template <typename T>
    T GetUniform(const std::string &alias)
    {
        auto schemaHandle = Renderer::Instance().GetBuiltInMaterialSchema(shaderFamily);
        auto schema = Renderer::Instance().GetShaderMaterialSchema(schemaHandle);
        auto paramPath = schema->GetAliasFullPath(alias);
        return GetUniformByPath<T>(paramPath);
    }

    template <typename T>
    T GetUniformByPath(const std::string &path);

    void SetTexture2D(const std::string &alias, AssetHandle handle)
    {
        auto schemaHandle = Renderer::Instance().GetBuiltInMaterialSchema(shaderFamily);
        auto schema = Renderer::Instance().GetShaderMaterialSchema(schemaHandle);
        auto texture2DName = schema->GetAliasFullPath(alias);
        auto binding = schema->GetResourceBinding(texture2DName);
        resources[binding] = {ShaderReflection::SamplerType::Texture2D, handle};
    }

    AssetHandle GetTexture2D(const std::string &alias) const 
    {
        auto schemaHandle = Renderer::Instance().GetBuiltInMaterialSchema(shaderFamily);
        auto schema = Renderer::Instance().GetShaderMaterialSchema(schemaHandle);
        auto texture2DName = schema->GetAliasFullPath(alias);
        auto binding = schema->GetResourceBinding(texture2DName);
        return resources.at(binding).value().handle; 
    }
    
    void SerializeParam(const std::string &paramAlias, ISerializer &serializer);
    void SerializeStructParam(ISerializer &serializer, const ShaderReflection::Struct *structType, std::string paramPath);
    void SerializeFundamentalParam(ISerializer &serializer, const ShaderReflection::Fundamental *fundamentalType, std::string paramPath);
    void SerializeArrayParam(ISerializer &serializer, const ShaderReflection::Array *arrayType, std::string paramPath);
    void SerializeUniformParam(ISerializer &serializer, const ShaderReflection::DataType *type, std::string paramPath);
    void SerializeSampledImage(ISerializer &serializer, const ShaderReflection::SampledImage *imageType, const std::string &paramAlias);

  private:
    std::vector<std::optional<MaterialAssetResource>> resources;
    std::string name;
    ShaderFamily shaderFamily;
};

}

#include "resource/material_asset.tpp"