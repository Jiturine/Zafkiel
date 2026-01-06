#include "material_asset.h"
#include "asset_manager.h"
#include "shader_asset.h"
#include "core/meta/serializer/yaml_serializer.h"
#include "function/render/renderer.h"

namespace Zafkiel
{
MaterialAsset::MaterialAsset(AssetHandle handle, const std::string &name, const std::string &data)
    : Asset(handle), name(name)
{
    YamlDeserializer materialData(data);

    shaderFamily = materialData["ShaderFamily"].As<ShaderFamily>();
    
    schema = Renderer::GetBuiltInMaterialSchema(shaderFamily);

    for (auto &[paramName, paramType] : schema->GetParameterTypes())
    {
        if (paramType->GetCategory() == ShaderReflection::ResourceTypeCategory::UniformBlock)
        {
            parameters[paramName] = MaterialAssetParameter(paramType->As<ShaderReflection::UniformBlock>());
        }
        else if (paramType->GetCategory() == ShaderReflection::ResourceTypeCategory::SampledImage)
        {
            // parameters[paramName] = MaterialAssetParameter(); // TODO: default val
        }
    }

    for (auto &[paramAlias, param] : materialData["Parameters"].MapItems())
    {
        DeserializeParamAlias(paramAlias.As<std::string>(), param);
    }
}

MaterialAsset::MaterialAsset(AssetHandle handle, const std::string &name, ShaderFamily shaderFamily)
    : Asset(handle), name(name), shaderFamily(shaderFamily)
{
    schema = Renderer::GetBuiltInMaterialSchema(shaderFamily);

    for (auto &[paramName, paramType] : schema->GetParameterTypes())
    {
        if (paramType->GetCategory() == ShaderReflection::ResourceTypeCategory::UniformBlock)
        {
            parameters[paramName] = MaterialAssetParameter(paramType->As<ShaderReflection::UniformBlock>());
        }
        else if (paramType->GetCategory() == ShaderReflection::ResourceTypeCategory::SampledImage)
        {
            // parameters[paramName] = MaterialAssetParameter(); // TODO: default val
        }
    }
}

bool MaterialAsset::HasParam(const std::string &paramAlias)
{
    if (!schema->HasAlias(paramAlias)) return false;
    
    auto paramPath = schema->GetAliasPath(paramAlias);
    if (schema->GetParameterTypeCategory(paramPath.elems[0].name) == ShaderReflection::ResourceTypeCategory::UniformBlock)
    {
        return true; // TODO: change this
    }
    else if (schema->GetParameterTypeCategory(paramPath.elems[0].name) == ShaderReflection::ResourceTypeCategory::SampledImage)
    {
        auto paramType = schema->GetParameterTypes().at(paramPath.elems[0].name);
        return HasSampledImage(paramType->As<ShaderReflection::SampledImage>(), paramAlias);
    }
}

bool MaterialAsset::HasSampledImage(const ShaderReflection::SampledImage *imageType, const std::string &paramAlias)
{
    return HasTexture2D(paramAlias);
}

std::string MaterialAsset::Serialize()
{
    YamlSerializer serializer;
    serializer.BeginMap();
    serializer.Key("ShaderFamily").Value(ShaderFamily::BlinnPhong);
    serializer.Key("Parameters").BeginMap();
    for (auto &[alias, path] : schema->GetAliases())
    {
        if (HasParam(alias))
        {
            serializer.Key(alias);
            SerializeParam(alias, serializer);
        }
    }
    serializer.EndMap().EndMap();
    return serializer.c_str();
}

void MaterialAsset::DeserializeParamAlias(const std::string &paramAlias, const IDeserializer &param)
{
    if (schema->HasAlias(paramAlias))
    {
        auto paramPath = schema->GetAliasPath(paramAlias);
        if (schema->GetParameterTypeCategory(paramPath.elems[0].name) == ShaderReflection::ResourceTypeCategory::UniformBlock)
        {
            auto paramType = schema->GetUniformParameterType(paramPath);
            DeserializeUniformParam(param, paramType, paramPath);
        }
        else if (schema->GetParameterTypeCategory(paramPath.elems[0].name) == ShaderReflection::ResourceTypeCategory::SampledImage)
        {
            auto paramType = schema->GetParameterTypes().at(paramPath.elems[0].name);
            DeserializeSampledImage(param, paramType->As<ShaderReflection::SampledImage>(), paramAlias);
        }
    }
}

void MaterialAsset::DeserializeSampledImage(const IDeserializer &param, const ShaderReflection::SampledImage *imageType, const std::string &imageName)
{
    if (imageType->GetSamplerType() == ShaderReflection::SamplerType::Texture2D)
    {
        SetTexture2D(imageName, param.As<AssetHandle>());
    }
}

void MaterialAsset::DeserializeFundamentalParam(const IDeserializer &param, const ShaderReflection::Fundamental *fundamentalType, RenderResourceParameterPath paramPath)
{
    ShaderReflection::FundamentalKind kind = param["Kind"].As<ShaderReflection::FundamentalKind>();
    switch (kind)
    {
        using enum ShaderReflection::FundamentalKind;
    case Float: SetUniform(paramPath, param["Value"].As<float>()); break;
    case Float2: SetUniform(paramPath, param["Value"].As<vec2>()); break;
    case Float3: SetUniform(paramPath, param["Value"].As<vec3>()); break;
    case Float4: SetUniform(paramPath, param["Value"].As<vec4>()); break;
    case Mat3: SetUniform(paramPath, param["Value"].As<mat3>()); break;
    case Mat4: SetUniform(paramPath, param["Value"].As<mat4>()); break;
    case Int: SetUniform(paramPath, param["Value"].As<int>()); break;
    case Int2: SetUniform(paramPath, param["Value"].As<ivec2>()); break;
    case Int3: SetUniform(paramPath, param["Value"].As<ivec3>()); break;
    case Int4: SetUniform(paramPath, param["Value"].As<ivec4>()); break;
    case UInt: SetUniform(paramPath, param["Value"].As<uint32_t>()); break;
    case UInt2: SetUniform(paramPath, param["Value"].As<uvec2>()); break;
    case UInt3: SetUniform(paramPath, param["Value"].As<uvec3>()); break;
    case UInt4: SetUniform(paramPath, param["Value"].As<uvec4>()); break;
    case Bool: SetUniform(paramPath, param["Value"].As<bool>()); break;
    default:
        Log::Error("Unknown RenderResource Param!");
        return;
    }
}
void MaterialAsset::DeserializeStructParam(const IDeserializer &param, const ShaderReflection::Struct *structType, RenderResourceParameterPath paramPath)
{
    for (auto &field : structType->GetFields())
    {
        if (!param[field->GetName()].IsNull())
        {
            paramPath.elems.push_back({ RenderResourceParameterPath::PathElemType::Indent, field->GetName() });
            DeserializeUniformParam(param[field->GetName()], field->GetTypeInfo(), paramPath);
            paramPath.elems.pop_back();
        }
    }
}
void MaterialAsset::DeserializeUniformParam(const IDeserializer &param, const ShaderReflection::DataType *paramType, RenderResourceParameterPath paramPath)
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

void MaterialAsset::DeserializeArrayParam(const IDeserializer &param, const ShaderReflection::Array *arrayType, RenderResourceParameterPath paramPath)
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
    if (schema->HasAlias(paramAlias))
    {
        auto paramPath = schema->GetAliasPath(paramAlias);
        if (schema->GetParameterTypeCategory(paramPath.elems[0].name) == ShaderReflection::ResourceTypeCategory::UniformBlock)
        {
            auto paramType = schema->GetUniformParameterType(paramPath);
            SerializeUniformParam(serializer, paramType, paramPath);
        }
        else if (schema->GetParameterTypeCategory(paramPath.elems[0].name) == ShaderReflection::ResourceTypeCategory::SampledImage)
        {
            auto paramType = schema->GetParameterTypes().at(paramPath.elems[0].name);
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

void MaterialAsset::SerializeFundamentalParam(ISerializer &serializer, const ShaderReflection::Fundamental *fundamentalType, RenderResourceParameterPath paramPath)
{
    switch (fundamentalType->GetKind())
    {
        using enum ShaderReflection::FundamentalKind;
    case Float: serializer.Value(GetUniform<float>(paramPath)); break;
    case Float2: serializer.Value(GetUniform<vec2>(paramPath)); break;
    case Float3: serializer.Value(GetUniform<vec3>(paramPath)); break;
    case Float4: serializer.Value(GetUniform<vec4>(paramPath)); break;
    case Mat3: serializer.Value(GetUniform<mat3>(paramPath)); break;
    case Mat4: serializer.Value(GetUniform<mat4>(paramPath)); break;
    case Int: serializer.Value(GetUniform<int>(paramPath)); break;
    case Int2: serializer.Value(GetUniform<ivec2>(paramPath)); break;
    case Int3: serializer.Value(GetUniform<ivec3>(paramPath)); break;
    case Int4: serializer.Value(GetUniform<ivec4>(paramPath)); break;
    case UInt: serializer.Value(GetUniform<uint32_t>(paramPath)); break;
    case UInt2: serializer.Value(GetUniform<uvec2>(paramPath)); break;
    case UInt3: serializer.Value(GetUniform<uvec3>(paramPath)); break;
    case UInt4: serializer.Value(GetUniform<uvec4>(paramPath)); break;
    case Bool: serializer.Value(GetUniform<bool>(paramPath)); break;
    default:
        Log::Error("Unknown RenderResource Param!");
        return;
    }
}
void MaterialAsset::SerializeStructParam(ISerializer &serializer, const ShaderReflection::Struct *structType, RenderResourceParameterPath paramPath)
{
    serializer.BeginMap();
    for (auto &field : structType->GetFields())
    {
        serializer.Key(field->GetName());
        paramPath.elems.push_back({ RenderResourceParameterPath::PathElemType::Indent, field->GetName() });
        SerializeUniformParam(serializer, field->GetTypeInfo(), paramPath);
        paramPath.elems.pop_back();
    }
    serializer.EndMap();
}
void MaterialAsset::SerializeUniformParam(ISerializer &serializer, const ShaderReflection::DataType *paramType, RenderResourceParameterPath paramPath)
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

void MaterialAsset::SerializeArrayParam(ISerializer &serializer, const ShaderReflection::Array *arrayType, RenderResourceParameterPath paramPath)
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

template <typename T>
T MaterialAsset::GetUniform(const RenderResourceParameterPath &path)
{
    auto uniformBlock = schema->GetParameterTypes().at(path.elems[0].name)->As<ShaderReflection::UniformBlock>();
    auto *curLayout = &uniformBlock->GetLayout();
    for (uint32_t i = 1; i < path.elems.size(); i++)
    {
        if (curLayout->dataType->GetCategory() == ShaderReflection::DataTypeCategory::Fundamental)
        {
            if (RenderResourceTrait<T>::type == curLayout->dataType->As<ShaderReflection::Fundamental>()->GetKind())
            {
                if constexpr (RenderResourceTrait<T>::type != ShaderFundamentalType::Mat3 && 
                    RenderResourceTrait<T>::type != ShaderFundamentalType::Mat4)
                {
                    auto uniformData = parameters[uniformBlock->GetName()].uniformBuffer.Data<uint8_t>();
                    T res;
                    memcpy(&res, uniformData + curLayout->offset, sizeof(T));
                    return res;
                }
                else if constexpr (RenderResourceTrait<T>::type == ShaderFundamentalType::Mat3) 
                {
                    auto uniformData = parameters[uniformBlock->GetName()].uniformBuffer.Data<uint8_t>();
                    auto src = uniformData + curLayout->offset;
                    T res;
                    for (int col = 0; col < 3; ++col)
                    {
                        memcpy(
                            &res[col][0], 
                            src + col * curLayout->matrixStride,
                            sizeof(float) * 3
                        );
                    }
                    return res;
                }
                else if constexpr (RenderResourceTrait<T>::type == ShaderFundamentalType::Mat4) 
                {
                    auto uniformData = parameters[uniformBlock->GetName()].uniformBuffer.Data<uint8_t>();
                    auto src = uniformData + curLayout->offset;
                    T res;
                    for (int col = 0; col < 4; ++col)
                    {
                        memcpy(
                            &res[col][0], 
                            src + col * curLayout->matrixStride,
                            sizeof(float) * 4
                        );
                    }
                    return res;
                }
            }
        }
        auto elem = path.elems[i];
        if (elem.type == RenderResourceParameterPath::PathElemType::Indent && curLayout->dataType->GetCategory() == ShaderReflection::DataTypeCategory::Struct)
        {
            auto &fields = curLayout->dataType->As<ShaderReflection::Struct>()->GetFields();
            for (auto &field : fields)
            {
                if (field->GetName() == elem.name)
                {
                    auto it = std::find_if(curLayout->children.begin(), curLayout->children.end(), [&elem](const ShaderReflection::UniformFieldLayout &layout) {
                        return layout.name == elem.name;
                    });
                    if (it != curLayout->children.end()) 
                    {
                        curLayout = &(*it);
                    }
                    else  
                    {
                        Log::Error("Field and Layout don't match!");
                        return {};
                    }
                    break;
                }
            }
        }
        else if (elem.type == RenderResourceParameterPath::PathElemType::Index && curLayout->dataType->GetCategory() == ShaderReflection::DataTypeCategory::Array)
        {
            // TODO: deal with array;
            Log::Error("TODO array");
            return {};
        }
    }
    Log::Error("Unknown Uniform Param Type!");
    return {};
}

}