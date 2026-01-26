#pragma once
#include "platform/filesystem/filesystem.h"
#include "function/render/uniform_buffer.h"
#include "function/render/vertex_buffer.h"
#include "function/render/shader.h"
#include "function/render/vertex_module.h"
#include "function/render/fragment_module.h"

namespace Zafkiel
{

struct GraphicsShaderReflection
{
    VertexBufferLayout vertexInput;
};

class GraphicsShader final : public Shader
{
  public:
    GraphicsShader(RenderHandle vertexModule, RenderHandle fragmentModule, Scope<ShaderBackend> backend)
        : vertexModule(vertexModule), fragmentModule(fragmentModule), Shader(std::move(backend)) {}

    virtual ShaderType GetShaderType() const override { return ShaderType::Graphics; }

    const GraphicsShaderReflection &GetReflection() const { return reflection; }
    GraphicsShaderReflection &GetReflection() { return reflection; }

    RenderHandle GetVertexModule() const { return vertexModule; }
    RenderHandle GetFragmentModule() const { return fragmentModule; }

    void CombineModules(Borrow<VertexModule> vertexModule);

  protected:
    GraphicsShaderReflection reflection;
    RenderHandle vertexModule;
    RenderHandle fragmentModule;
};
}
