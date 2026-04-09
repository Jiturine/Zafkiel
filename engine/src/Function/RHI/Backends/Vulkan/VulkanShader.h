#pragma once
#include "Function/RHI/RHIResources.h"
#include "Function/RHI/ShaderCompiler/SpirvReflection.h"
#include <vulkan/vulkan_raii.hpp>
#include <spirv_cross/spirv_cross.hpp>

namespace Zafkiel
{

class VulkanDevice;

vk::ShaderStageFlagBits ShaderTypeToVulkanType(ShaderType type);

const ShaderReflection::Fundamental *SPIRTypeToShaderDataType(const spirv_cross::SPIRType &type);

vk::Format ShaderDataTypeToVulkanFormat(ShaderFundamentalType type);

const uint32 ShaderDataTypeSize(ShaderFundamentalType type);

class VulkanShader
{
  public:
    VulkanShader(Buffer code, VulkanDevice &device, ShaderResourceTable &shaderResourceTable, ShaderKey shaderKey);

    ~VulkanShader();

    vk::raii::ShaderModule &GetShaderModule() { return shaderModule; }

    ShaderResourceTable &GetShaderResourceTable() { return SRT; }

    ShaderKey GetShaderKey() const { return shaderKey; }

  private:
    vk::raii::ShaderModule shaderModule;

    ShaderResourceTable &SRT;
  
    ShaderKey shaderKey;

    VulkanDevice &device;
};

class VulkanVertexShader : public RHIVertexShader, public VulkanShader
{
  public:
    VulkanVertexShader(Buffer code, VulkanDevice &device, Scope<ShaderReflection::SpirvReflection> spvReflection, ShaderKey shaderKey);
};

class VulkanFragmentShader : public RHIFragmentShader, public VulkanShader
{
  public:
    VulkanFragmentShader(Buffer code, VulkanDevice &device, Scope<ShaderReflection::SpirvReflection> spvReflection, ShaderKey shaderKey)
        : RHIFragmentShader(spvReflection->GetShaderResourceTable()), 
          VulkanShader(code, device, *shaderResourceTable.get(), shaderKey) {}
};

class VulkanShaderRegistry
{
  public:
    template <typename TShader>
    requires std::is_base_of_v<VulkanShader, TShader>
    Ref<TShader> CreateShader(Buffer code, VulkanDevice &device, Scope<ShaderReflection::SpirvReflection> spvReflection)
    {
        ShaderKey shaderKey = CalcShaderKey(code);
        
        if (auto shader = GetShader<TShader>(shaderKey))
        {
            return shader;
        }

        auto result = CreateRef<TShader>(code, device, MoveTemp(spvReflection), shaderKey);
        shaderMap[shaderKey] = result.get();

        return result;
    }
    
    template <typename TShader>
    TShader *GetShader(ShaderKey shaderKey)
    {
        if (shaderMap.contains(shaderKey))
        {
            return static_cast<TShader *>(shaderMap[shaderKey]);
        }
        return nullptr;
    }

    VulkanShader *GetShader(ShaderKey shaderKey)
    {
        if (shaderMap.contains(shaderKey))
        {
            return shaderMap[shaderKey];
        }
        return nullptr;
    }

    void RemoveShader(ShaderKey shaderKey);

  private:
    std::unordered_map<ShaderKey, VulkanShader *> shaderMap;
};

}
