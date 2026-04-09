#pragma once
#include "Function/RHI/RHIResources.h"
#include <spirv_cross/spirv_cross.hpp>

namespace Zafkiel::ShaderReflection
{

class SpirvReflection
{
  public:
    SpirvReflection(Buffer spirvCode, std::vector<std::string> staticUniformBuffers, GraphicsAPI api, ShaderStage::Stage stage);

    Scope<ShaderResourceTable> GetShaderResourceTable() { return MoveTemp(shaderResourceTable); }

    std::optional<ShaderVertexInput> GetVertexInputLayout();

  private:
    const ShaderReflection::DataType *ParseDataType(const spirv_cross::Compiler &compiler, uint32 typeId);
    const ShaderReflection::DataType *ParseStructType(const spirv_cross::Compiler &compiler, const spirv_cross::SPIRType &structType);
    const ShaderReflection::DataType *ParseArrayType(const spirv_cross::Compiler &compiler, const spirv_cross::SPIRType &arrayType);
    const ShaderReflection::DataType *ParseFundamentalType(const spirv_cross::Compiler &compiler, const spirv_cross::SPIRType &fundamentalType);

    void CalculateUniformFieldLayout(const spirv_cross::Compiler &compiler, const spirv_cross::SPIRType &type, const ShaderReflection::DataType *customType, ShaderReflection::UniformFieldLayout &layout);

    Scope<ShaderResourceTable> shaderResourceTable;
    Scope<spirv_cross::Compiler> compiler; // 用于后续访问
};

}
