#pragma once
#include "shader_reflection.h"
#include "platform/filesystem/filesystem.h"
namespace Zafkiel
{

enum class ShaderType
{
    Graphics,
    Compute
};

class ShaderBackend
{
  public:
    virtual ~ShaderBackend() = default;
};

class Shader
{
  public:
    Shader(Scope<ShaderBackend> backend) : backend(std::move(backend)) {}
    virtual ~Shader() = default;
    virtual ShaderType GetShaderType() const = 0;

    Observer<ShaderBackend> GetShaderBackend() { return backend; }
    const Observer<ShaderBackend> GetShaderBackend() const { return backend; }

  private:
    Scope<ShaderBackend> backend;

};

}
