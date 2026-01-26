#pragma once
#include "function/render/render_handle.h"

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
    virtual ~Shader() = default;
    virtual ShaderType GetShaderType() const = 0;
    Shader(Scope<ShaderBackend> backend) : backend(std::move(backend)) {}

    Borrow<ShaderBackend> GetBackend() const { return Borrow(backend); }

  private:
    Scope<ShaderBackend> backend;

};

}
