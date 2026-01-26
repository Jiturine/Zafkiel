#include "resource/material_asset.h"
#include "resource/asset_manager.h"
#include "resource/shader_asset.h"
#include "core/meta/serializer/yaml_serializer.h"
#include "function/render/renderer.h"

namespace Zafkiel
{
MaterialAsset::MaterialAsset(AssetHandle handle, const std::string &name, const std::string &data)
    : Asset(handle), name(name)
{
    YamlDeserializer materialData(data);

    shaderFamily = materialData["ShaderFamily"].As<ShaderFamily>();
    
    auto schemaHandle = Renderer::Instance().GetBuiltInMaterialSchema(shaderFamily);
    auto schema = Renderer::Instance().GetShaderMaterialSchema(schemaHandle);

    resources.resize(schema->GetResourceTypeInfos().size());
    for (auto [binding, resourceTypeInfo] : std::views::enumerate(schema->GetResourceTypeInfos()))
    {
        if (!resourceTypeInfo) continue;
        if (resourceTypeInfo.value().type->GetCategory() == ShaderReflection::ResourceTypeCategory::UniformBlock)
        {
            resources[binding] = MaterialAssetResource(resourceTypeInfo.value().type->As<ShaderReflection::UniformBlock>());
        }
    }

    for (auto &[paramAlias, param] : materialData["Parameters"].MapItems())
    {
        DeserializeParam(paramAlias.As<std::string>(), param);
    }
}

MaterialAsset::MaterialAsset(AssetHandle handle, const std::string &name, ShaderFamily shaderFamily)
    : Asset(handle), name(name), shaderFamily(shaderFamily)
{
    auto schemaHandle = Renderer::Instance().GetBuiltInMaterialSchema(shaderFamily);
    auto schema = Renderer::Instance().GetShaderMaterialSchema(schemaHandle);

    for (auto [binding, resourceTypeInfo] : std::views::enumerate(schema->GetResourceTypeInfos()))
    {
        if (!resourceTypeInfo) continue;
        if (resourceTypeInfo.value().type->GetCategory() == ShaderReflection::ResourceTypeCategory::UniformBlock)
        {
            resources[binding] = MaterialAssetResource(resourceTypeInfo.value().type->As<ShaderReflection::UniformBlock>());
        }
    }
}

std::string MaterialAsset::Serialize()
{
    YamlSerializer serializer;
    serializer.BeginMap();
    serializer.Key("ShaderFamily").Value(ShaderFamily::BlinnPhong);
    serializer.Key("Parameters").BeginMap();
    auto schemaHandle = Renderer::Instance().GetBuiltInMaterialSchema(shaderFamily);
    auto schema = Renderer::Instance().GetShaderMaterialSchema(schemaHandle);
    for (auto &[alias, path] : schema->GetAliases())
    {
        // TODO: 考虑alias没有实际值的情况
        serializer.Key(alias);
        SerializeParam(alias, serializer);
    }
    serializer.EndMap().EndMap();
    return serializer.c_str();
}

void MaterialAsset::DeserializeParam(const std::string &paramAlias, const IDeserializer &param)
{
    auto schemaHandle = Renderer::Instance().GetBuiltInMaterialSchema(shaderFamily);
    auto schema = Renderer::Instance().GetShaderMaterialSchema(schemaHandle);
    if (schema->HasAlias(paramAlias))
    {
        auto paramPath = schema->GetAliasFullPath(paramAlias);
        auto resourceName = GetResourceName(paramPath); 
        if (schema->GetResourceTypeCategory(resourceName) == ShaderReflection::ResourceTypeCategory::UniformBlock)
        {
            auto paramType = schema->GetUniformParameterType(paramPath);
            DeserializeUniformParam(param, paramType, paramPath);
        }
        else if (schema->GetResourceTypeCategory(resourceName) == ShaderReflection::ResourceTypeCategory::SampledImage)
        {
            auto binding = schema->GetResourceBinding(resourceName);
            auto paramType = schema->GetResourceTypeInfos().at(binding).value();
            DeserializeSampledImage(param, paramType.type->As<ShaderReflection::SampledImage>(), paramAlias);
        }
    }
    else  
    {
        Log::Error("Unknown Parameter Alias!");
    }
}

void MaterialAsset::DeserializeSampledImage(const IDeserializer &param, const ShaderReflection::SampledImage *imageType, const std::string &imageName)
{
    if (imageType->GetSamplerType() == ShaderReflection::SamplerType::Texture2D)
    {
        SetTexture2D(imageName, param.As<AssetHandle>());
    }
}

void MaterialAsset::DeserializeFundamentalParam(const IDeserializer &param, const ShaderReflection::Fundamental *fundamentalType, std::string paramPath)
{
    ShaderReflection::FundamentalKind kind = param["Kind"].As<ShaderReflection::FundamentalKind>();
    switch (kind)
    {
        using enum ShaderReflection::FundamentalKind;
    case Float: SetUniformByPath(paramPath, param["Value"].As<float>()); break;
    case Float2: SetUniformByPath(paramPath, param["Value"].As<vec2>()); break;
    case Float3: SetUniformByPath(paramPath, param["Value"].As<vec3>()); break;
    case Float4: SetUniformByPath(paramPath, param["Value"].As<vec4>()); break;
    case Mat3: SetUniformByPath(paramPath, param["Value"].As<mat3>()); break;
    case Mat4: SetUniformByPath(paramPath, param["Value"].As<mat4>()); break;
    case Int: SetUniformByPath(paramPath, param["Value"].As<int>()); break;
    case Int2: SetUniformByPath(paramPath, param["Value"].As<ivec2>()); break;
    case Int3: SetUniformByPath(paramPath, param["Value"].As<ivec3>()); break;
    case Int4: SetUniformByPath(paramPath, param["Value"].As<ivec4>()); break;
    case UInt: SetUniformByPath(paramPath, param["Value"].As<uint32_t>()); break;
    case UInt2: SetUniformByPath(paramPath, param["Value"].As<uvec2>()); break;
    case UInt3: SetUniformByPath(paramPath, param["Value"].As<uvec3>()); break;
    case UInt4: SetUniformByPath(paramPath, param["Value"].As<uvec4>()); break;
    case Bool: SetUniformByPath(paramPath, param["Value"].As<bool>()); break;
    default:
        Log::Error("Unknown RenderResource Param!");
        return;
    }
}
void MaterialAsset::DeserializeStructParam(const IDeserializer &param, const ShaderReflection::Struct *structType, std::string paramPath)
{
    for (auto &field : structType->GetFields())
    {
        if (!param[field->GetName()].IsNull())
        {
            DeserializeUniformParam(param[field->GetName()], field->GetTypeInfo(), paramPath + field->GetName());
        }
    }
}
void MaterialAsset::DeserializeUniformParam(const IDeserializer &param, const ShaderReflection::DataType *paramType, std::string paramPath)
{
    if (paramType->GetCategory() == ShaderReflection::DataTypeCategory::Fundamental)
    {
        DeserializeFundamentalParam(param, paramType->As<ShaderReflection::Fundamental>(), paramPath);
    }
    else if (paramType->GetCategory() == ShaderReflection::DataTypeCategory::Struct)
    {
        DeserializeStructParam(param, paramType->As<ShaderReflection::Struct>(), paramPath);
    }
    else if (paramType->GetCategory() == ShaderReflection::DataTypeCategory::Array)
    {
        DeserializeArrayParam(param, paramType->As<ShaderReflection::Array>(), paramPath);
    }
}

void MaterialAsset::DeserializeArrayParam(const IDeserializer &param, const ShaderReflection::Array *arrayType, std::string paramPath)
{
    // if (customTypes.contains(arrayName))
    // {
    //     uint32_t elemIndex = 0;
    //     for (auto &elemData : param["Elems"].SeqElems())
    //     {
    //         DeserializeParam(elemData, std::format("{}[{}]", path, elemIndex));
    //         elemIndex++;
    //     }
    // }
}

void MaterialAsset::SerializeParam(const std::string &paramAlias, ISerializer &serializer)
{
    auto schemaHandle = Renderer::Instance().GetBuiltInMaterialSchema(shaderFamily);
    auto schema = Renderer::Instance().GetShaderMaterialSchema(schemaHandle);
    if (schema->HasAlias(paramAlias))
    {
        auto paramPath = schema->GetAliasFullPath(paramAlias);
        auto resourceName = GetResourceName(paramPath); 
        auto binding = schema->GetResourceBinding(resourceName);
        if (schema->GetResourceTypeCategory(resourceName) == ShaderReflection::ResourceTypeCategory::UniformBlock)
        {
            auto paramType = schema->GetUniformParameterType(paramPath);
            SerializeUniformParam(serializer, paramType, paramPath);
        }
        else if (schema->GetResourceTypeCategory(resourceName) == ShaderReflection::ResourceTypeCategory::SampledImage)
        {
            auto paramType = schema->GetResourceTypeInfos().at(binding).value().type;
            SerializeSampledImage(serializer, paramType->As<ShaderReflection::SampledImage>(), paramAlias);
        }
    }
}

void MaterialAsset::SerializeSampledImage(ISerializer &serializer, const ShaderReflection::SampledImage *imageType, const std::string &paramAlias)
{
    if (imageType->GetSamplerType() == ShaderReflection::SamplerType::Texture2D)
    {
        serializer.Value<AssetHandle>(GetTexture2D(paramAlias));
    }
}

void MaterialAsset::SerializeFundamentalParam(ISerializer &serializer, const ShaderReflection::Fundamental *fundamentalType, std::string paramPath)
{
    switch (fundamentalType->GetKind())
    {
        using enum ShaderReflection::FundamentalKind;
    case Float: serializer.Value(GetUniformByPath<float>(paramPath)); break;
    case Float2: serializer.Value(GetUniformByPath<vec2>(paramPath)); break;
    case Float3: serializer.Value(GetUniformByPath<vec3>(paramPath)); break;
    case Float4: serializer.Value(GetUniformByPath<vec4>(paramPath)); break;
    case Mat3: serializer.Value(GetUniformByPath<mat3>(paramPath)); break;
    case Mat4: serializer.Value(GetUniformByPath<mat4>(paramPath)); break;
    case Int: serializer.Value(GetUniformByPath<int>(paramPath)); break;
    case Int2: serializer.Value(GetUniformByPath<ivec2>(paramPath)); break;
    case Int3: serializer.Value(GetUniformByPath<ivec3>(paramPath)); break;
    case Int4: serializer.Value(GetUniformByPath<ivec4>(paramPath)); break;
    case UInt: serializer.Value(GetUniformByPath<uint32_t>(paramPath)); break;
    case UInt2: serializer.Value(GetUniformByPath<uvec2>(paramPath)); break;
    case UInt3: serializer.Value(GetUniformByPath<uvec3>(paramPath)); break;
    case UInt4: serializer.Value(GetUniformByPath<uvec4>(paramPath)); break;
    case Bool: serializer.Value(GetUniformByPath<bool>(paramPath)); break;
    default:
        Log::Error("Unknown RenderResource Param!");
        return;
    }
}
void MaterialAsset::SerializeStructParam(ISerializer &serializer, const ShaderReflection::Struct *structType, std::string paramPath)
{
    serializer.BeginMap();
    for (auto &field : structType->GetFields())
    {
        serializer.Key(field->GetName());
        SerializeUniformParam(serializer, field->GetTypeInfo(), paramPath + field->GetName());
    }
    serializer.EndMap();
}
void MaterialAsset::SerializeUniformParam(ISerializer &serializer, const ShaderReflection::DataType *paramType, std::string paramPath)
{
    if (paramType->GetCategory() == ShaderReflection::DataTypeCategory::Fundamental)
    {
        SerializeFundamentalParam(serializer, paramType->As<ShaderReflection::Fundamental>(), paramPath);
    }
    else if (paramType->GetCategory() == ShaderReflection::DataTypeCategory::Struct)
    {
        SerializeStructParam(serializer, paramType->As<ShaderReflection::Struct>(), paramPath);
    }
    else if (paramType->GetCategory() == ShaderReflection::DataTypeCategory::Array)
    {
        SerializeArrayParam(serializer, paramType->As<ShaderReflection::Array>(), paramPath);
    }
}

void MaterialAsset::SerializeArrayParam(ISerializer &serializer, const ShaderReflection::Array *arrayType, std::string paramPath)
{
    // if (customTypes.contains(arrayName))
    // {
    //     uint32_t elemIndex = 0;
    //     for (auto &elemData : param["Elems"].SeqElems())
    //     {
    //         DeserializeParam(elemData, std::format("{}[{}]", path, elemIndex));
    //         elemIndex++;
    //     }
    // }
}
}