#pragma once
#include "function/render/shader_module.h"
#include "platform/filesystem/filesystem.h"
#include <shaderc/shaderc.hpp>

namespace Zafkiel 
{

class GlslCompiler 
{
  public:
    GlslCompiler(const Path &rootPath) : rootPath(rootPath) {}
    ScopedBuffer Compile(const std::string &source, GraphicsAPI api, ShaderStage stage, const std::string &name);

  private:
    ScopedBuffer CompileVulkanSpv(const std::string &shaderCode, const std::string &shaderName, shaderc_shader_kind kind);
    ScopedBuffer CompileOpenGLSpv(Buffer vulkanSpv, const std::string &shaderName, shaderc_shader_kind kind);
  
    Path rootPath;
};

}