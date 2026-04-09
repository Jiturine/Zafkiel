#include "Function/RHI/ShaderCompiler/SpirvReflection.h"
#include "Function/RHI/ShaderReflection.h"

namespace Zafkiel::ShaderReflection
{

static std::unordered_map<ShaderStage::Stage, uint32> shaderStageBindingOffset
{
    {ShaderStage::Vertex, 0},
    {ShaderStage::Fragment, 16},
    {ShaderStage::Geometry, 32},
};

SpirvReflection::SpirvReflection(Buffer spirvCode, std::vector<std::string> staticUniformBuffers, GraphicsAPI api, ShaderStage::Stage stage)
{
    compiler = CreateScope<spirv_cross::Compiler>(spirvCode.Data<uint32>(), spirvCode.Size<uint32>());

    shaderResourceTable = CreateScope<ShaderResourceTable>();
    shaderResourceTable->staticUniformBuffers = MoveTemp(staticUniformBuffers);

    // 获取着色器资源
    spirv_cross::ShaderResources resources = compiler->get_shader_resources();

    // 处理 Uniform Buffers
    for (const auto &ubo : resources.uniform_buffers)
    {
        const auto &type = compiler->get_type(ubo.type_id);
        std::string name = compiler->get_name(ubo.id);

        std::string typeName = ubo.name;

        // 获取 set 和 binding
        uint32 set = compiler->get_decoration(ubo.id, spv::DecorationDescriptorSet);
        uint32 binding = compiler->get_decoration(ubo.id, spv::DecorationBinding);

        if (api == GraphicsAPI::OpenGL)
        {
            binding -= shaderStageBindingOffset[stage];
        }

        // 解析结构体类型
        auto structType = ParseStructType(*compiler, type)->As<ShaderReflection::Struct>();

        // 计算 Uniform Buffer 布局
        ShaderReflection::UniformFieldLayout layout;
        CalculateUniformFieldLayout(*compiler, type, structType, layout);

        // 获取布局规则
        ShaderReflection::LayoutRule layoutRule = ShaderReflection::LayoutRule::Std140; // SPIR-V 默认使用 std140

        // 创建 UniformBlock 类型
        shaderResourceTable->customResourceTypes[typeName] = CreateScope<ShaderReflection::UniformBlock>(typeName, layoutRule, structType, layout);

        // 存储资源信息
        if (binding >= shaderResourceTable->resourceTypeInfos.size())
        {
            shaderResourceTable->resourceTypeInfos.resize(binding + 1);
        }

        shaderResourceTable->resourceTypeInfos[binding] = {name, shaderResourceTable->customResourceTypes[typeName].get()};
        shaderResourceTable->resourceNameToBinding[name] = binding;
    }

    // 处理 Sampled Images (纹理)
    for (const auto &image : resources.sampled_images)
    {
        std::string name = compiler->get_name(image.id);

        // 获取 set 和 binding
        uint32 set = compiler->get_decoration(image.id, spv::DecorationDescriptorSet);
        uint32 binding = compiler->get_decoration(image.id, spv::DecorationBinding);

        if (api == GraphicsAPI::OpenGL)
        {
            binding -= shaderStageBindingOffset[stage];
        }

        const auto &type = compiler->get_type(image.type_id);
        const auto &imageType = type.image;

        // 确定采样器类型
        ShaderReflection::SamplerType samplerType = ShaderReflection::SamplerType::Texture2D;
        if (imageType.dim == spv::Dim2D)
        {
            samplerType = ShaderReflection::SamplerType::Texture2D;
        }
        // else if (imageType.dim == spv::DimCube)
        // {
        //     samplerType = ShaderReflection::SamplerType::TextureCube;
        // }
        // 可以添加更多类型...

        // 存储资源信息
        if (binding >= shaderResourceTable->resourceTypeInfos.size())
        {
            shaderResourceTable->resourceTypeInfos.resize(binding + 1);
        }

        shaderResourceTable->resourceTypeInfos[binding] = {name, ShaderReflection::GetSampledImageType(samplerType)};
        shaderResourceTable->resourceNameToBinding[name] = binding;
    }

    // 统计 buffer 和 image 数量
    uint32 bufferCount = 0, imageCount = 0;
    for (auto resourceInfo : shaderResourceTable->resourceTypeInfos)
    {
        if (resourceInfo.type->GetCategory() == ShaderReflection::ResourceTypeCategory::UniformBlock)
        {
            bufferCount++;
        }
        else if (resourceInfo.type->GetCategory() == ShaderReflection::ResourceTypeCategory::SampledImage)
        {
            imageCount++;
        }
    }
    shaderResourceTable->bufferCount = bufferCount;
    shaderResourceTable->imageCount = imageCount;
}

std::optional<ShaderVertexInput> SpirvReflection::GetVertexInputLayout()
{
    spirv_cross::ShaderResources resources = compiler->get_shader_resources();

    if (resources.stage_inputs.empty())
    {
        return std::nullopt;
    }

    ShaderVertexInput vertexInput;
    uint32 offset = 0;

    for (const auto &input : resources.stage_inputs)
    {
        const auto &type = compiler->get_type(input.type_id);

        // 解析基础数据类型
        auto dataType = ParseFundamentalType(*compiler, type);
        if (!dataType)
        {
            Log::Error("Failed to parse vertex input type!");
            continue;
        }

        auto fundamentalKind = dataType->As<ShaderReflection::Fundamental>()->GetKind();

        ShaderVertexInput::InputLayout elementLayout
        {
            .name = compiler->get_name(input.id),
            .location = compiler->get_decoration(input.id, spv::DecorationLocation),
            .size = GetSizeOfShaderFundamentalType(fundamentalKind),
            .offset = offset,
            .type = fundamentalKind
        };

        vertexInput.inputs.push_back(elementLayout);
        offset += elementLayout.size;
    }

    vertexInput.stride = offset;
    return vertexInput;
}

const ShaderReflection::DataType *SpirvReflection::ParseDataType(const spirv_cross::Compiler &compiler, uint32 typeId)
{
    const auto &type = compiler.get_type(typeId);

    if (!type.array.empty())
    {
        return ParseArrayType(compiler, type);
    }
    else if (type.basetype == spirv_cross::SPIRType::Struct)
    {
        return ParseStructType(compiler, type);
    }
    else if (type.basetype == spirv_cross::SPIRType::Float || type.basetype == spirv_cross::SPIRType::Int || type.basetype == spirv_cross::SPIRType::UInt)
    {
        return ParseFundamentalType(compiler, type);
    }
    else
    {
        Log::Error("Unknown data type!");
        return nullptr;
    }
}

const ShaderReflection::DataType *SpirvReflection::ParseStructType(const spirv_cross::Compiler &compiler, const spirv_cross::SPIRType &structType)
{
    // 尝试从类型 ID 获取结构体名称
    std::string structName = compiler.get_name(structType.self);

    if (structName.empty())
    {
        // 为匿名结构体生成名称
        static uint32 anonymousStructCounter = 0;
        structName = "AnonymousStruct_" + std::to_string(anonymousStructCounter++);
    }

    // 检查是否已经解析过
    if (shaderResourceTable->customDataTypes.contains(structName))
    {
        return shaderResourceTable->customDataTypes[structName].get();
    }

    // 创建新的结构体类型
    auto structTypeObj = CreateScope<ShaderReflection::Struct>(structName);

    // 解析每个成员
    for (uint32 i = 0; i < structType.member_types.size(); i++)
    {
        uint32 memberTypeId = structType.member_types[i];
        std::string memberName = compiler.get_member_name(structType.self, i);

        auto memberType = ParseDataType(compiler, memberTypeId);
        if (!memberType)
        {
            Log::Error("Failed to parse struct member type!");
            continue;
        }

        auto field = std::make_shared<ShaderReflection::Field>(memberName, structTypeObj.get(), memberType);
        structTypeObj->AddField(field);
    }

    shaderResourceTable->customDataTypes[structName] = MoveTemp(structTypeObj);
    return shaderResourceTable->customDataTypes[structName].get();
}

const ShaderReflection::DataType *SpirvReflection::ParseArrayType(const spirv_cross::Compiler &compiler, const spirv_cross::SPIRType &arrayType)
{
    // TODO: 实现数组类型解析
    Log::Error("Array type parsing not implemented yet!");
    return nullptr;
}

const ShaderReflection::DataType *SpirvReflection::ParseFundamentalType(const spirv_cross::Compiler &compiler, const spirv_cross::SPIRType &fundamentalType)
{
    using namespace spirv_cross;
    using namespace ShaderReflection;

    if (fundamentalType.basetype == SPIRType::Float)
    {
        if (fundamentalType.columns == 1)
        {
            switch (fundamentalType.vecsize)
            {
                case 1: return GetFundamentalType(FundamentalKind::Float);
                case 2: return GetFundamentalType(FundamentalKind::Float2);
                case 3: return GetFundamentalType(FundamentalKind::Float3);
                case 4: return GetFundamentalType(FundamentalKind::Float4);
            }
        }
        else if (fundamentalType.columns == fundamentalType.vecsize)
        {
            // 矩阵类型
            switch (fundamentalType.columns)
            {
                case 3: return GetFundamentalType(FundamentalKind::Mat3);
                case 4: return GetFundamentalType(FundamentalKind::Mat4);
            }
        }
    }
    else if (fundamentalType.basetype == SPIRType::Int)
    {
        if (fundamentalType.columns == 1)
        {
            switch (fundamentalType.vecsize)
            {
                case 1: return GetFundamentalType(FundamentalKind::Int);
                case 2: return GetFundamentalType(FundamentalKind::Int2);
                case 3: return GetFundamentalType(FundamentalKind::Int3);
                case 4: return GetFundamentalType(FundamentalKind::Int4);
            }
        }
    }
    else if (fundamentalType.basetype == SPIRType::UInt)
    {
        if (fundamentalType.columns == 1)
        {
            switch (fundamentalType.vecsize)
            {
                case 1: return GetFundamentalType(FundamentalKind::UInt);
                case 2: return GetFundamentalType(FundamentalKind::UInt2);
                case 3: return GetFundamentalType(FundamentalKind::UInt3);
                case 4: return GetFundamentalType(FundamentalKind::UInt4);
            }
        }
    }

    Log::Error("Unsupported fundamental type!");
    return nullptr;
}

void SpirvReflection::CalculateUniformFieldLayout(const spirv_cross::Compiler &compiler, const spirv_cross::SPIRType &type, const ShaderReflection::DataType *customType, ShaderReflection::UniformFieldLayout &layout)
{
    // 清空现有数据
    layout.children.clear();

    if (type.basetype == spirv_cross::SPIRType::Struct)
    {
        auto structType = customType->As<ShaderReflection::Struct>();

        // 计算结构体的总大小
        layout.size = compiler.get_declared_struct_size(type);

        // 递归计算每个成员的布局
        for (uint32 i = 0; i < type.member_types.size(); i++)
        {
            const auto &memberType = compiler.get_type(type.member_types[i]);
            auto memberCustomType = structType->GetFields()[i]->GetTypeInfo();

            // 获取成员的 offset（相对于结构体起始位置）
            uint32 memberOffset = compiler.type_struct_member_offset(type, i);

            // 创建成员布局
            ShaderReflection::UniformFieldLayout memberLayout;
            memberLayout.offset = memberOffset;

            // 根据成员类型计算大小和递归布局
            if (memberType.basetype == spirv_cross::SPIRType::Struct)
            {
                // 嵌套结构体：递归计算其内部布局
                CalculateUniformFieldLayout(compiler, memberType, memberCustomType, memberLayout);
            }
            else
            {
                // 基础类型：直接获取大小
                memberLayout.size = compiler.get_declared_struct_member_size(type, i);

                // 处理数组
                if (!memberType.array.empty())
                {
                    memberLayout.isArray = true;
                    memberLayout.arraySize = memberType.array[0];

                    // 如果是数组，获取元素步长
                    if (memberType.array.size() > 1)
                    {
                        // 多维数组，当前暂不支持
                        Log::Warn("Multi-dimensional arrays not supported yet!");
                    }
                    else
                    {
                        // 一维数组，获取元素步长
                        memberLayout.elemStride = compiler.type_struct_member_array_stride(type, i);
                    }

                    // 数组总大小 = 元素大小 * 数组长度
                    memberLayout.size = memberLayout.elemStride * memberLayout.arraySize;
                }

                // 处理矩阵
                if (memberType.columns > 1)
                {
                    // 获取矩阵步长
                    memberLayout.matrixStride = compiler.type_struct_member_matrix_stride(type, i);
                }
            }

            layout.children.push_back(memberLayout);
        }
    }
    else
    {
        Log::Error("CalculateUniformFieldLayout: Expected Struct type, got something else!");
    }
}

}
