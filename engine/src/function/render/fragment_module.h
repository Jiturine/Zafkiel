#pragma once
#include "function/render/shader_module.h"

namespace Zafkiel 
{

class FragmentModule final : public ShaderModule
{
  public:
    FragmentModule(Buffer code, Scope<ShaderModuleBackend> backend)
        : ShaderModule(code, std::move(backend)) {}

    virtual ShaderStage GetShaderStage() const override { return ShaderStage::Fragment; }
};

}
