#pragma once
#include "shader_module.h"

namespace Zafkiel 
{
class FragmentModuleBackend
{
  public:
    virtual ~FragmentModuleBackend() = default;
};

class FragmentModule final : public ShaderModule
{
  public:
    FragmentModule(Buffer code, Scope<ShaderModuleBackend> shaderModuleBackend, Scope<FragmentModuleBackend> fragmentModuleBackend)
        : ShaderModule(code, std::move(shaderModuleBackend)), backend(std::move(fragmentModuleBackend)) {}

    virtual ShaderStage GetShaderStage() const override { return ShaderStage::Fragment; }

    Observer<FragmentModuleBackend> GetFragmentModuleBackend() { return backend; }
    const Observer<FragmentModuleBackend> GetFragmentModuleBackend() const { return backend; }

  private:
    Scope<FragmentModuleBackend> backend;
};

}