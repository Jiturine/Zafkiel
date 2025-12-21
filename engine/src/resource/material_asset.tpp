#pragma once
#include "material_asset.h"

namespace Zafkiel 
{

template <typename T>
void MaterialAsset::SetUniform(const RenderResourceParameterPath &paramPath, T value)
{
    auto uniformBlock = schema->GetParameterTypes().at(paramPath.elems[0].name)->As<ShaderReflection::UniformBlock>();
    auto *curLayout = &uniformBlock->GetLayout();
    for (uint32_t i = 1; i < paramPath.elems.size(); i++)
    {
        if (curLayout->dataType->GetCategory() == ShaderReflection::DataTypeCategory::Fundamental)
        {
            if (RenderResourceTrait<T>::type == curLayout->dataType->As<ShaderReflection::Fundamental>()->GetKind())
            {
                if constexpr (RenderResourceTrait<T>::type != ShaderFundamentalType::Mat3 && 
                    RenderResourceTrait<T>::type != ShaderFundamentalType::Mat4)
                {
                    auto uniformData = parameters[uniformBlock->GetName()].uniformBuffer.Data<uint8_t>();
                    memcpy(uniformData + curLayout->offset, &value, sizeof(T));
                }
                else if constexpr (RenderResourceTrait<T>::type == ShaderFundamentalType::Mat3) 
                {
                    auto uniformData = parameters[uniformBlock->GetName()].uniformBuffer.Data<uint8_t>();
                    auto dst = uniformData + curLayout->offset;
                    for (int col = 0; col < 3; ++col)
                    {
                        memcpy(
                            dst + col * curLayout->matrixStride, 
                            &value[col][0],
                            sizeof(float) * 3
                        );
                    }
                }
                else if constexpr (RenderResourceTrait<T>::type == ShaderFundamentalType::Mat4) 
                {
                    auto uniformData = parameters[uniformBlock->GetName()].uniformBuffer.Data<uint8_t>();
                    auto dst = uniformData + curLayout->offset;
                    for (int col = 0; col < 4; ++col)
                    {
                        memcpy(
                            dst + col * curLayout->matrixStride, 
                            &value[col][0],
                            sizeof(float) * 4
                        );
                    }
                }
            }
        }
        auto elem = paramPath.elems[i];
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
                        return;
                    }
                    break;
                }
            }
        }
        else if (elem.type == RenderResourceParameterPath::PathElemType::Index && curLayout->dataType->GetCategory() == ShaderReflection::DataTypeCategory::Array)
        {
            // TODO: deal with array;
            Log::Error("TODO array");
            return;
        }
    }
}
    
}