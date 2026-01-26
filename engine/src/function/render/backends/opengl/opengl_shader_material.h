#pragma once
#include "function/render/shader_material.h"
#include "function/render/shader_material_template.h"
#include "function/render/uniform_buffer.h"

namespace Zafkiel
{

class OpenGLShaderMaterialBackend final : public ShaderMaterialBackend
{
  public:
    OpenGLShaderMaterialBackend(std::vector<std::optional<Scope<UniformBuffer>>> uniformBuffers)
        : uniformBuffers(std::move(uniformBuffers)) {}

    virtual void SetTexture2D(uint32_t binding, RenderHandle handle) override {}

    Borrow<UniformBuffer> GetUniformBuffer(uint32_t binding) const
    {
        return Borrow(uniformBuffers[binding].value());
    }
    
  private:
    std::vector<std::optional<Scope<UniformBuffer>>> uniformBuffers;
};

}
