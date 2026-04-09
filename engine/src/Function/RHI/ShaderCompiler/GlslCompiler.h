#pragma once
#include "Function/RHI/RHIResources.h"
#include "Function/RHI/ShaderCompiler/SpirvReflection.h"
#include "Platform/Filesystem/Filesystem.h"
#include <shaderc/shaderc.hpp>

namespace Zafkiel 
{

class GlslCompiler
{
  public:
    GlslCompiler(const Path &rootPath) : rootPath(rootPath) {}

    ScopedBuffer Compile(const std::string &source, GraphicsAPI api, ShaderType type, const std::string &name);
  
    Scope<ShaderReflection::SpirvReflection> GetReflection() { return MoveTemp(spvReflection); }

  private:
    std::string Preprocess(const std::string &sourceCode, ShaderType shaderType);

    ScopedBuffer CompileVulkanSpv(const std::string &shaderCode, const std::string &shaderName, shaderc_shader_kind kind);

    ScopedBuffer CompileOpenGLSpv(Buffer vulkanSpv, const std::string &shaderName, shaderc_shader_kind kind);

    std::vector<std::string> staticUniformBuffers;

    Scope<ShaderReflection::SpirvReflection> spvReflection;

    Path rootPath;
};

}