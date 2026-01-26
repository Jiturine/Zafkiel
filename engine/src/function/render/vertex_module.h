#pragma once
#include "function/render/shader_module.h"

namespace Zafkiel
{

class VertexModule final : public ShaderModule
{
  public:
    VertexModule(Buffer code, Scope<ShaderModuleBackend> backend)
        : ShaderModule(code, std::move(backend)) {}

    virtual ShaderStage GetShaderStage() const override { return ShaderStage::Vertex; }
};

}
