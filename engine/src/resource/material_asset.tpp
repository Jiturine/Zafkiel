#pragma once
#include "material_asset.h"

namespace Zafkiel 
{
template <typename T>
struct ShaderMaterialTrait;

template <> struct ShaderMaterialTrait<int> { static constexpr ShaderFundamentalType type = ShaderFundamentalType::Int; };
template <> struct ShaderMaterialTrait<ivec2> { static constexpr ShaderFundamentalType type = ShaderFundamentalType::Int2; };
template <> struct ShaderMaterialTrait<ivec3> { static constexpr ShaderFundamentalType type = ShaderFundamentalType::Int3; };
template <> struct ShaderMaterialTrait<ivec4> { static constexpr ShaderFundamentalType type = ShaderFundamentalType::Int4; };
template <> struct ShaderMaterialTrait<float> { static constexpr ShaderFundamentalType type = ShaderFundamentalType::Float; };
template <> struct ShaderMaterialTrait<vec2> { static constexpr ShaderFundamentalType type = ShaderFundamentalType::Float2; };
template <> struct ShaderMaterialTrait<vec3> { static constexpr ShaderFundamentalType type = ShaderFundamentalType::Float3; };
template <> struct ShaderMaterialTrait<vec4> { static constexpr ShaderFundamentalType type = ShaderFundamentalType::Float4; };
template <> struct ShaderMaterialTrait<mat3> { static constexpr ShaderFundamentalType type = ShaderFundamentalType::Mat3; };
template <> struct ShaderMaterialTrait<mat4> { static constexpr ShaderFundamentalType type = ShaderFundamentalType::Mat4; };
template <> struct ShaderMaterialTrait<bool> { static constexpr ShaderFundamentalType type = ShaderFundamentalType::Bool; };
template <> struct ShaderMaterialTrait<uint32_t> { static constexpr ShaderFundamentalType type = ShaderFundamentalType::UInt; };
template <> struct ShaderMaterialTrait<uvec2> { static constexpr ShaderFundamentalType type = ShaderFundamentalType::UInt2; };
template <> struct ShaderMaterialTrait<uvec3> { static constexpr ShaderFundamentalType type = ShaderFundamentalType::UInt3; };
template <> struct ShaderMaterialTrait<uvec4> { static constexpr ShaderFundamentalType type = ShaderFundamentalType::UInt4; };

template <typename T>
void MaterialAsset::SetUniformByPath(const std::string &paramPath, T value)
{
    auto parsedPath = ParseParameterPath(paramPath);
    auto schemaHandle = Renderer::Instance().GetBuiltInMaterialSchema(shaderFamily);
    auto schema = Renderer::Instance().GetShaderMaterialSchema(schemaHandle);
    auto binding = schema->GetResourceBinding(parsedPath.elems[0].name);
    auto uniformBlock = schema->GetResourceTypeInfos().at(binding).value().type->As<ShaderReflection::UniformBlock>();
    auto uniformBufferData = resources[binding].value().uniformBuffer.Data<uint8_t>();
    const ShaderReflection::DataType *curType = uniformBlock->GetType();
    for (uint32_t i = 1; i < parsedPath.elems.size(); i++)
    {
        auto elem = parsedPath.elems[i];
        if (elem.type == ShaderMaterialParameterPath::PathElemType::Indent && curType->GetCategory() == ShaderReflection::DataTypeCategory::Struct)
        {
            auto &fields = curType->As<ShaderReflection::Struct>()->GetFields();
            const auto it = std::find_if(fields.begin(), fields.end(), [&](const auto &field) { return field->GetName() == elem.name; });
            if (it == fields.end())
            {
                Log::Error("Invalid Parameter Path!");
                return;
            }
            curType = (*it)->GetTypeInfo();
        }
        else if (elem.type == ShaderMaterialParameterPath::PathElemType::Index && curType->GetCategory() == ShaderReflection::DataTypeCategory::Array)
        {
            // TODO: deal with array;
            Log::Error("TODO array");
            return;
        }
    }
    if (curType->GetCategory() == ShaderReflection::DataTypeCategory::Fundamental)
    {
        auto &fieldLayout = uniformBlock->GetFieldLayout(curType);
        if (ShaderMaterialTrait<T>::type == curType->As<ShaderReflection::Fundamental>()->GetKind())
        {
            if constexpr (ShaderMaterialTrait<T>::type != ShaderFundamentalType::Mat3 && 
                ShaderMaterialTrait<T>::type != ShaderFundamentalType::Mat4)
            {
                memcpy(uniformBufferData + fieldLayout.offset, &value, sizeof(T));
            }
            else if constexpr (ShaderMaterialTrait<T>::type == ShaderFundamentalType::Mat3) 
            {
                auto dst = uniformBufferData + fieldLayout.offset;
                for (int col = 0; col < 3; ++col)
                {
                    memcpy(
                        dst + col * fieldLayout.matrixStride, 
                        &value[col][0],
                        sizeof(float) * 3
                    );
                }
            }
            else if constexpr (ShaderMaterialTrait<T>::type == ShaderFundamentalType::Mat4) 
            {
                auto dst = uniformBufferData + fieldLayout.offset;
                for (int col = 0; col < 4; ++col)
                {
                    memcpy(
                        dst + col * fieldLayout.matrixStride, 
                        &value[col][0],
                        sizeof(float) * 4
                    );
                }
            }
        }
    }
    else 
    {
        Log::Error("MaterialAsset::SetUniformByPath Unknown Uniform Param Type!");
    }
}

template <typename T>
T MaterialAsset::GetUniformByPath(const std::string &path)
{
    auto parsedPath = ParseParameterPath(path);
    auto schemaHandle = Renderer::Instance().GetBuiltInMaterialSchema(shaderFamily);
    auto schema = Renderer::Instance().GetShaderMaterialSchema(schemaHandle);
    auto binding = schema->GetResourceBinding(parsedPath.elems[0].name);
    auto uniformBlock = schema->GetResourceTypeInfos().at(binding).value().type->As<ShaderReflection::UniformBlock>();
    auto uniformBufferData = resources[binding].value().uniformBuffer.Data<uint8_t>();
    const ShaderReflection::DataType *curType = uniformBlock->GetType();
    for (uint32_t i = 1; i < parsedPath.elems.size(); i++)
    {
        auto elem = parsedPath.elems[i];
        if (elem.type == ShaderMaterialParameterPath::PathElemType::Indent && curType->GetCategory() == ShaderReflection::DataTypeCategory::Struct)
        {
            auto &fields = curType->As<ShaderReflection::Struct>()->GetFields();
            auto it = std::find_if(fields.begin(), fields.end(), [&](const auto &field) { return field->GetName() == elem.name; });
            if (it == fields.end())
            {
                Log::Error("Invalid Parameter Path!");
                return {};
            }
            curType = (*it)->GetTypeInfo();
        }
        else if (elem.type == ShaderMaterialParameterPath::PathElemType::Index && curType->GetCategory() == ShaderReflection::DataTypeCategory::Array)
        {
            // TODO: deal with array;
            Log::Error("TODO array");
            return {};
        }
    }
    if (curType->GetCategory() == ShaderReflection::DataTypeCategory::Fundamental)
    {
        auto &fieldLayout = uniformBlock->GetFieldLayout(curType);
        if (ShaderMaterialTrait<T>::type == curType->As<ShaderReflection::Fundamental>()->GetKind())
        {
            if constexpr (ShaderMaterialTrait<T>::type != ShaderFundamentalType::Mat3 && 
                ShaderMaterialTrait<T>::type != ShaderFundamentalType::Mat4)
            {
                T res;
                memcpy(&res, uniformBufferData + fieldLayout.offset, sizeof(T));
                return res;
            }
            else if constexpr (ShaderMaterialTrait<T>::type == ShaderFundamentalType::Mat3) 
            {
                auto src = uniformBufferData + fieldLayout.offset;
                T res;
                for (int col = 0; col < 3; ++col)
                {
                    memcpy(
                        &res[col][0], 
                        src + col * fieldLayout.matrixStride,
                        sizeof(float) * 3
                    );
                }
                return res;
            }
            else if constexpr (ShaderMaterialTrait<T>::type == ShaderFundamentalType::Mat4) 
            {
                auto src = uniformBufferData + fieldLayout.offset;
                T res;
                for (int col = 0; col < 4; ++col)
                {
                    memcpy(
                        &res[col][0], 
                        src + col * fieldLayout.matrixStride,
                        sizeof(float) * 4
                    );
                }
                return res;
            }
        }
    }
    Log::Error("MaterialAsset::GetUniformByPath Unknown Uniform Param Type!");
    return {};
}
    
}