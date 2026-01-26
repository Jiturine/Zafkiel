#pragma once
#include "function/render/object_shader_material.h"
#include "function/render/uniform_buffer.h"

namespace Zafkiel 
{
    
class OpenGLObjectShaderMaterialBackend final : public ObjectShaderMaterialBackend
{
  public:
    OpenGLObjectShaderMaterialBackend(std::vector<std::optional<Scope<UniformBuffer>>> uniformBuffers)
        : uniformBuffers(std::move(uniformBuffers)) {}

    const std::vector<std::optional<Scope<UniformBuffer>>> &GetUniformBuffers() const { return uniformBuffers; }

    Borrow<UniformBuffer> GetUniformBuffer(uint32_t binding) const
    {
        return Borrow(uniformBuffers[binding].value());
    }
  
  private:
    std::vector<std::optional<Scope<UniformBuffer>>> uniformBuffers;
};

}