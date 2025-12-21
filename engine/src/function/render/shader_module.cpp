#include "shader_module.h"
#include <spirv_cross/spirv_cross.hpp>

namespace Zafkiel
{

static const ShaderReflection::Fundamental *SPIRTypeToShaderDataType(const spirv_cross::SPIRType &type)
{
    switch (type.basetype)
    {
        using enum spirv_cross::SPIRType::BaseType;
    case Float:
        if (type.columns == 1 && type.vecsize > 1) // vector
        {
            if (type.vecsize == 2) return ShaderReflection::GetFundamentalType(ShaderReflection::FundamentalKind::Float2);
            if (type.vecsize == 3) return ShaderReflection::GetFundamentalType(ShaderReflection::FundamentalKind::Float3);
            if (type.vecsize == 4) return ShaderReflection::GetFundamentalType(ShaderReflection::FundamentalKind::Float4);
        }
        break;
    case UInt:
        if (type.columns == 1 && type.vecsize == 1)
        {
            return ShaderReflection::GetFundamentalType(ShaderReflection::FundamentalKind::UInt);
        }
        break;
    default: break;
    }
    Log::Error("Unsupported ShaderDataType!");
    return {};
}

static const uint32_t ShaderDataTypeSize(ShaderFundamentalType type)
{
    switch (type)
    {
        using enum ShaderFundamentalType;
    case Float: return 4;
    case Float2: return 2 * 4;
    case Float3: return 3 * 4;
    case Float4: return 4 * 4;
    case Int: return 4;
    case Int2: return 2 * 4;
    case Int3: return 3 * 4;
    case Int4: return 4 * 4;
    case UInt: return 4;
    case UInt2: return 2 * 4;
    case UInt3: return 3 * 4;
    case UInt4: return 4 * 4;
    default:
        Log::Error("Invalid Vertex Input!");
        return 0;
    }
}

void ShaderModule::Reflect(Buffer codeBuffer)
{
    spirv_cross::Compiler compiler(codeBuffer.Data<uint32_t>(), codeBuffer.Size<uint32_t>());
    spirv_cross::ShaderResources resources = compiler.get_shader_resources();

    // for (const auto &uniformBuffer : resources.uniform_buffers)
    // {
    //     auto &uniformBufferType = compiler.get_type(uniformBuffer.base_type_id);

    //     UniformBufferLayout uniformBufferLayout
    //     {
    //         .name = compiler.get_name(uniformBuffer.base_type_id),
    //         .binding = compiler.get_decoration(uniformBuffer.id, spv::DecorationBinding),
    //         .size = static_cast<uint32_t>(compiler.get_declared_struct_size(uniformBufferType)),
    //     };

    //     for (size_t i = 0; i < uniformBufferType.member_types.size(); i++)
    //     {
    //         UniformBufferMemberLayout member
    //         {
    //             .name = compiler.get_member_name(uniformBuffer.base_type_id, i),
    //             .offset = compiler.type_struct_member_offset(uniformBufferType, i),
    //             .size = static_cast<uint32_t>(compiler.get_declared_struct_member_size(uniformBufferType, i))
    //         };
    //         uniformBufferLayout.members.push_back(member);
    //     }
    //     reflection.uniformBuffers.push_back(uniformBufferLayout);
    // }

    // for (const auto &sampled_images : resources.sampled_images)
    // {
    //     SampledImageLayout layout
    //     {
    //         .name = compiler.get_name(sampled_images.id),
    //         .binding = compiler.get_decoration(sampled_images.id, spv::DecorationBinding),
    //     };
    //     reflection.sampledImages.push_back(layout);
    // }

    uint32_t offset = 0;
    for (const auto &input : resources.stage_inputs)
    {
        auto &type = compiler.get_type(input.type_id);
        auto dataType = SPIRTypeToShaderDataType(type)->GetKind();
        ShaderModuleReflection::InputLayout elementLayout
        {
            .name = compiler.get_name(input.id),
            .location = compiler.get_decoration(input.id, spv::DecorationLocation),
            .size = ShaderDataTypeSize(dataType),
            .offset = offset,
            .type = dataType
        };
        reflection.inputs.push_back(elementLayout);
        offset += elementLayout.size;
    }
}
}
