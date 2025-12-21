#pragma once
#include "object_render_resource.h"
#include "render_resource.h"

namespace Zafkiel 
{

template <typename T>
void ObjectRenderResource::SetUniform(uint32_t index, const RenderResourceParameterPath &paramPath, T value)
{
    auto uniformBlock = objectRenderResourceTemplate->GetParameterTypes().at(paramPath.elems[0].name)->As<ShaderReflection::UniformBlock>();
    auto *curLayout = &uniformBlock->GetLayout();
    for (size_t i = 0; ; i++)
    {
        if (curLayout->dataType->GetCategory() == ShaderReflection::DataTypeCategory::Fundamental &&
            RenderResourceTrait<T>::type == curLayout->dataType->As<ShaderReflection::Fundamental>()->GetKind())
        {
            if constexpr (RenderResourceTrait<T>::type != ShaderFundamentalType::Mat3 && 
                RenderResourceTrait<T>::type != ShaderFundamentalType::Mat4)
            {
                auto uniformData = parameters[uniformBlock->GetName()].dynamicUniformBuffer.Data<uint8_t>() + index * uniformBlock->GetLayout().size;
                memcpy(uniformData + curLayout->offset, &value, sizeof(T));
            }
            else if constexpr (RenderResourceTrait<T>::type == ShaderFundamentalType::Mat3) 
            {
                auto uniformData = parameters[uniformBlock->GetName()].dynamicUniformBuffer.Data<uint8_t>() + index * uniformBlock->GetLayout().size;
                auto dst = uniformData + curLayout->offset;
                for (int col = 0; col < 3; ++col)
                {
                    memcpy(dst + col * curLayout->matrixStride, &value[col][0], sizeof(float) * 3);
                }
            }
            else if constexpr (RenderResourceTrait<T>::type == ShaderFundamentalType::Mat4) 
            {
                auto uniformData = parameters[uniformBlock->GetName()].dynamicUniformBuffer.Data<uint8_t>() + index * uniformBlock->GetLayout().size;
                auto dst = uniformData + curLayout->offset;
                for (int col = 0; col < 4; ++col)
                {
                    memcpy(dst + col * curLayout->matrixStride, &value[col][0], sizeof(float) * 4);
                }
            }
            return;
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