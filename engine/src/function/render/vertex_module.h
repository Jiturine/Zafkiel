#pragma once
#include "shader_module.h"

namespace Zafkiel
{

class VertexModuleBackend
{
  public:
    virtual ~VertexModuleBackend() = default;
};

class VertexModule final : public ShaderModule
{
  public:
    VertexModule(Buffer code, Scope<ShaderModuleBackend> shaderModuleBackend, Scope<VertexModuleBackend> vertexModuleBackend)
        : ShaderModule(code, std::move(shaderModuleBackend)), backend(std::move(vertexModuleBackend)) {}

    virtual ShaderStage GetShaderStage() const override { return ShaderStage::Vertex; }

    Observer<VertexModuleBackend> GetVertexModuleBackend() { return backend; }
    const Observer<VertexModuleBackend> GetVertexModuleBackend() const { return backend; }

  private:
    Scope<VertexModuleBackend> backend;
};

}
