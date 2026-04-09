#include "Function/RHI/Backends/Vulkan/VulkanShader.h"
#include "Function/RHI/Backends/Vulkan/VulkanDevice.h"
#include "Core/Base/Crc.h"

namespace Zafkiel
{

vk::ShaderStageFlagBits ShaderTypeToVulkanType(ShaderType type)
{
    switch (type)
    {
        using enum ShaderType;
    case Vertex: return vk::ShaderStageFlagBits::eVertex;
    case Fragment: return vk::ShaderStageFlagBits::eFragment;
    case Geometry: return vk::ShaderStageFlagBits::eGeometry;
    default:
        Log::Error("Unsupported Shader Stage!");
        return vk::ShaderStageFlagBits::eAll;
    }
}

vk::Format ShaderDataTypeToVulkanFormat(ShaderFundamentalType type)
{
    switch (type)
    {
        using enum ShaderReflection::FundamentalKind;
    case Float2:
        return vk::Format::eR32G32Sfloat;
    case Float3:
        return vk::Format::eR32G32B32Sfloat;
    case Float4:
        return vk::Format::eR32G32B32A32Sfloat;
    case Int:
        return vk::Format::eR32Sint;
    case UInt:
        return vk::Format::eR32Uint;
    default:
        Log::Error("Failed to Convert to Vulkan Format!");
        return vk::Format::eUndefined;
    }
}

const ShaderReflection::Fundamental *SPIRTypeToShaderDataType(const spirv_cross::SPIRType &type)
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
    case Int:
        if (type.columns == 1 && type.vecsize == 1)
        {
            return ShaderReflection::GetFundamentalType(ShaderReflection::FundamentalKind::Int);
        }
        break;
    default: break;
    }
    Log::Error("Unsupported ShaderDataType!");
    return {};
}

const uint32 ShaderDataTypeSize(ShaderFundamentalType type)
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

void VulkanShaderRegistry::RemoveShader(ShaderKey shaderKey)
{
    shaderMap.erase(shaderKey);
}

VulkanShader::VulkanShader(Buffer code, VulkanDevice &device, ShaderResourceTable &shaderResourceTable, ShaderKey shaderKey)
    : shaderModule(nullptr), device(device), shaderKey(shaderKey), SRT(shaderResourceTable)
{
    vk::ShaderModuleCreateInfo createInfo;
    createInfo.setCode({code.Size<uint32_t>(), code.Data<uint32_t>()});
    shaderModule = device.GetHandle().createShaderModule(createInfo);
}

VulkanShader::~VulkanShader()
{
    device.GetShaderRegistry().RemoveShader(shaderKey);
}

VulkanVertexShader::VulkanVertexShader(Buffer code, VulkanDevice &device, Scope<ShaderReflection::SpirvReflection> spvReflection, ShaderKey shaderKey)
    : RHIVertexShader(spvReflection->GetShaderResourceTable()), 
      VulkanShader(code, device, *shaderResourceTable.get(), shaderKey)
{
    spirv_cross::Compiler compiler(code.Data<uint32>(), code.Size<uint32>());
    spirv_cross::ShaderResources resources = compiler.get_shader_resources();

    uint32 offset = 0;
    for (const auto &input : resources.stage_inputs)
    {
        auto &type = compiler.get_type(input.type_id);
        auto dataType = SPIRTypeToShaderDataType(type)->GetKind();
        ShaderVertexInput::InputLayout elementLayout
        {
            .name = compiler.get_name(input.id),
            .location = compiler.get_decoration(input.id, spv::DecorationLocation),
            .size = ShaderDataTypeSize(dataType),
            .offset = offset,
            .type = dataType
        };
        vertexInput.inputs.push_back(elementLayout);
        offset += elementLayout.size;
    }
    vertexInput.stride = offset;
}

}