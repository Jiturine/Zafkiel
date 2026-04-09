#include "Function/RHI/RHIResources.h"
#include "Core/Base/Crc.h"

namespace Zafkiel
{

RHIBuffer::RHIBuffer(const RHIBufferDesc &desc)
    : RHIResource(RHIResourceType::Buffer), desc(desc)
{
    
}

ShaderKey CalcShaderKey(Buffer shaderCode)
{
	uint32 shaderCodeLen = shaderCode.Size<uint8>();
	uint32 shaderCodeCRC = Crc::MemCrc32(shaderCode);
	uint64 shaderKey = ((uint64)shaderCodeLen | ((uint64)shaderCodeCRC << 32));
    return shaderKey;
}

UniformParameterPath UniformParameterPath::Parse(const std::string &path)
{
    UniformParameterPath result;
    uint32_t i = 0;

    auto readIdent = [&]() {
        uint32_t start = i;
        while (i < path.size() &&
              (isalnum(path[i]) || path[i] == '_'))
            i++;
        return path.substr(start, i - start);
    };

    while (i < path.size()) {
        if (isalpha(path[i]) || path[i] == '_') {
            std::string ident = readIdent();
            result.elems.push_back({
                .type = PathElemType::Indent,
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
                .type = PathElemType::Index,
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

UniformBufferContent::UniformBufferContent(const ShaderReflection::UniformBlock *uniformBlock)
    : buffer(uniformBlock->GetSize()), uniformBlock(uniformBlock)
{
}

void UniformBufferContent::SetParameter(const std::string &parameterPath, Buffer data, ShaderFundamentalType type)
{
    auto parsedPath = UniformParameterPath::Parse(parameterPath);

    uint8 *uniformBufferData = buffer.Data<uint8>();

    // 从顶层开始
    const ShaderReflection::DataType *curType = uniformBlock->GetType();
    const ShaderReflection::UniformFieldLayout *currentLayout = &uniformBlock->GetLayout();
    uint32 totalOffset = 0;

    for (auto &elem : parsedPath.elems)
    {
        if (elem.type == UniformParameterPath::PathElemType::Indent && curType->GetCategory() == ShaderReflection::DataTypeCategory::Struct)
        {
            auto &fields = curType->As<ShaderReflection::Struct>()->GetFields();
            auto it = std::find_if(fields.begin(), fields.end(), [&](const auto &field) { return field->GetName() == elem.name; });
            if (it == fields.end())
            {
                Log::Error("Invalid Parameter Path: field '{}' not found!", elem.name);
                return;
            }

            // 在 children 中查找对应的布局
            size_t fieldIndex = std::distance(fields.begin(), it);
            if (fieldIndex >= currentLayout->children.size())
            {
                Log::Error("Invalid Parameter Path: layout mismatch for field '{}'!", elem.name);
                return;
            }

            // 累积偏移并更新当前布局
            totalOffset += currentLayout->children[fieldIndex].offset;
            currentLayout = &currentLayout->children[fieldIndex];
            curType = (*it)->GetTypeInfo();
        }
        else if (elem.type == UniformParameterPath::PathElemType::Index && currentLayout->isArray)
        {
            // 处理数组索引
            if (elem.index >= currentLayout->arraySize)
            {
                Log::Error("Invalid Parameter Path: array index {} out of bounds (size: {})!", elem.index, currentLayout->arraySize);
                return;
            }

            // 计算数组元素的偏移
            totalOffset += elem.index * currentLayout->elemStride;

            // 数组元素的类型需要从 curType 中获取
            // TODO: 这里需要 Array 类型支持，暂时跳过
            Log::Error("Array indexing not fully implemented yet!");
            return;
        }
    }

    if (curType->GetCategory() == ShaderReflection::DataTypeCategory::Fundamental)
    {
        auto kind = curType->As<ShaderReflection::Fundamental>()->GetKind();
        switch (kind)
        {
            using enum ShaderFundamentalType;
        case Float: case Float2: case Float3: case Float4:
        case Int: case Int2: case Int3: case Int4:
        case UInt: case UInt2: case UInt3: case UInt4:
        case Bool:
            memcpy(uniformBufferData + totalOffset, data.Data<void>(), data.Size<uint8>());
            break;
        case Mat3:
            {
                mat3 matrix;
                memcpy(&matrix, data.Data<void>(), data.Size<uint8>());
                auto dst = uniformBufferData + totalOffset;
                for (int col = 0; col < 3; ++col)
                {
                    memcpy(
                        dst + col * currentLayout->matrixStride,
                        &matrix[col][0],
                        sizeof(float) * 3
                    );
                }
                break;
            }
        case Mat4:
            {
                mat4 matrix;
                memcpy(&matrix, data.Data<void>(), data.Size<uint8>());
                auto dst = uniformBufferData + totalOffset;
                for (int col = 0; col < 4; ++col)
                {
                    memcpy(
                        dst + col * currentLayout->matrixStride,
                        &matrix[col][0],
                        sizeof(float) * 4
                    );
                }
                break;
            }
        default:
            Log::Error("Unknown Uniform Param Type!");
            return;
        }
    }
    else
    {
        Log::Error("Invalid Parameter Path: expected fundamental type, got {}", static_cast<int>(curType->GetCategory()));
    }
}

ScopedBuffer UniformBufferContent::GetParameter(const std::string parameterPath, ShaderFundamentalType type)
{
    auto parsedPath = UniformParameterPath::Parse(parameterPath);

    uint8 *uniformBufferData = buffer.Data<uint8>();

    // 从顶层开始
    const ShaderReflection::DataType *curType = uniformBlock->GetType();
    const ShaderReflection::UniformFieldLayout *currentLayout = &uniformBlock->GetLayout();
    uint32 totalOffset = 0;

    for (auto &elem : parsedPath.elems)
    {
        if (elem.type == UniformParameterPath::PathElemType::Indent && curType->GetCategory() == ShaderReflection::DataTypeCategory::Struct)
        {
            auto &fields = curType->As<ShaderReflection::Struct>()->GetFields();
            auto it = std::find_if(fields.begin(), fields.end(), [&](const auto &field) { return field->GetName() == elem.name; });
            if (it == fields.end())
            {
                Log::Error("Invalid Parameter Path: field '{}' not found!", elem.name);
                return {};
            }

            // 在 children 中查找对应的布局
            size_t fieldIndex = std::distance(fields.begin(), it);
            if (fieldIndex >= currentLayout->children.size())
            {
                Log::Error("Invalid Parameter Path: layout mismatch for field '{}'!", elem.name);
                return {};
            }

            // 累积偏移并更新当前布局
            totalOffset += currentLayout->children[fieldIndex].offset;
            currentLayout = &currentLayout->children[fieldIndex];
            curType = (*it)->GetTypeInfo();
        }
        else if (elem.type == UniformParameterPath::PathElemType::Index && currentLayout->isArray)
        {
            // 处理数组索引
            if (elem.index >= currentLayout->arraySize)
            {
                Log::Error("Invalid Parameter Path: array index {} out of bounds (size: {})!", elem.index, currentLayout->arraySize);
                return {};
            }

            // 计算数组元素的偏移
            totalOffset += elem.index * currentLayout->elemStride;

            // 数组元素的类型需要从 curType 中获取
            // TODO: 这里需要 Array 类型支持，暂时跳过
            Log::Error("Array indexing not fully implemented yet!");
            return {};
        }
    }

    if (curType->GetCategory() == ShaderReflection::DataTypeCategory::Fundamental)
    {
        auto kind = curType->As<ShaderReflection::Fundamental>()->GetKind();
        switch (kind)
        {
            using enum ShaderFundamentalType;
        case Float: case Float2: case Float3: case Float4:
        case Int: case Int2: case Int3: case Int4:
        case UInt: case UInt2: case UInt3: case UInt4:
        case Bool:
            return ScopedBuffer(uniformBufferData + totalOffset, GetSizeOfShaderFundamentalType(kind));
        case Mat3:
            {
                auto src = uniformBufferData + totalOffset;
                mat3 res;
                for (int col = 0; col < 3; ++col)
                {
                    memcpy(
                        &res[col][0],
                        src + col * currentLayout->matrixStride,
                        sizeof(float) * 3
                    );
                }
                return ScopedBuffer((uint8 *)&res, currentLayout->matrixStride * 3);
            }
        case Mat4:
            {
                auto src = uniformBufferData + totalOffset;
                mat4 res;
                for (int col = 0; col < 4; ++col)
                {
                    memcpy(
                        &res[col][0],
                        src + col * currentLayout->matrixStride,
                        sizeof(float) * 4
                    );
                }
                return ScopedBuffer((uint8 *)&res, currentLayout->matrixStride * 4);
            }
        default:
            Log::Error("Unknown Uniform Param Type!");
            return {};
        }
    }
    else
    {
        Log::Error("Invalid Parameter Path: expected fundamental type, got {}", static_cast<int>(curType->GetCategory()));
        return {};
    }
}

DynamicUniformBufferContent::DynamicUniformBufferContent(uint32 maxSize, const ShaderReflection::UniformBlock *uniformBlock, uint32 minUniformBufferOffsetAlignment)
    : uniformBlock(uniformBlock), minUniformBufferOffsetAlignment(minUniformBufferOffsetAlignment),
      buffer(maxSize * AlignUp(uniformBlock->GetSize(), minUniformBufferOffsetAlignment))
{
}

void DynamicUniformBufferContent::SetParameter(uint32 index, const std::string &parameterPath, Buffer data, ShaderFundamentalType type)
{
    auto parsedPath = UniformParameterPath::Parse(parameterPath);

    uint8 *uniformBufferData = buffer.Data<uint8>() + index * AlignUp(uniformBlock->GetSize(), minUniformBufferOffsetAlignment);

    // 从顶层开始
    const ShaderReflection::DataType *curType = uniformBlock->GetType();
    const ShaderReflection::UniformFieldLayout *currentLayout = &uniformBlock->GetLayout();
    uint32 totalOffset = 0;

    for (auto &elem : parsedPath.elems)
    {
        if (elem.type == UniformParameterPath::PathElemType::Indent && curType->GetCategory() == ShaderReflection::DataTypeCategory::Struct)
        {
            auto &fields = curType->As<ShaderReflection::Struct>()->GetFields();
            auto it = std::find_if(fields.begin(), fields.end(), [&](const auto &field) { return field->GetName() == elem.name; });
            if (it == fields.end())
            {
                Log::Error("Invalid Parameter Path: field '{}' not found!", elem.name);
                return;
            }

            // 在 children 中查找对应的布局
            size_t fieldIndex = std::distance(fields.begin(), it);
            if (fieldIndex >= currentLayout->children.size())
            {
                Log::Error("Invalid Parameter Path: layout mismatch for field '{}'!", elem.name);
                return;
            }

            // 累积偏移并更新当前布局
            totalOffset += currentLayout->children[fieldIndex].offset;
            currentLayout = &currentLayout->children[fieldIndex];
            curType = (*it)->GetTypeInfo();
        }
        else if (elem.type == UniformParameterPath::PathElemType::Index && currentLayout->isArray)
        {
            // 处理数组索引
            if (elem.index >= currentLayout->arraySize)
            {
                Log::Error("Invalid Parameter Path: array index {} out of bounds (size: {})!", elem.index, currentLayout->arraySize);
                return;
            }

            // 计算数组元素的偏移
            totalOffset += elem.index * currentLayout->elemStride;

            // 数组元素的类型需要从 curType 中获取
            // TODO: 这里需要 Array 类型支持，暂时跳过
            Log::Error("Array indexing not fully implemented yet!");
            return;
        }
    }

    if (curType->GetCategory() == ShaderReflection::DataTypeCategory::Fundamental)
    {
        auto kind = curType->As<ShaderReflection::Fundamental>()->GetKind();
        switch (kind)
        {
            using enum ShaderFundamentalType;
        case Float: case Float2: case Float3: case Float4:
        case Int: case Int2: case Int3: case Int4:
        case UInt: case UInt2: case UInt3: case UInt4:
        case Bool:
            memcpy(uniformBufferData + totalOffset, data.Data<void>(), data.Size<uint8>());
            break;
        case Mat3:
            {
                mat3 matrix;
                memcpy(&matrix, data.Data<void>(), data.Size<uint8>());
                auto dst = uniformBufferData + totalOffset;
                for (int col = 0; col < 3; ++col)
                {
                    memcpy(
                        dst + col * currentLayout->matrixStride,
                        &matrix[col][0],
                        sizeof(float) * 3
                    );
                }
                break;
            }
        case Mat4:
            {
                mat4 matrix;
                memcpy(&matrix, data.Data<void>(), data.Size<uint8>());
                auto dst = uniformBufferData + totalOffset;
                for (int col = 0; col < 4; ++col)
                {
                    memcpy(
                        dst + col * currentLayout->matrixStride,
                        &matrix[col][0],
                        sizeof(float) * 4
                    );
                }
                break;
            }
        default:
            Log::Error("Unknown Uniform Param Type!");
            return;
        }
    }
    else
    {
        Log::Error("Invalid Parameter Path: expected fundamental type, got {}", static_cast<int>(curType->GetCategory()));
    }
}


}