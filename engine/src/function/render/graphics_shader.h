#pragma once
#include "platform/filesystem/filesystem.h"
#include "uniform_buffer.h"
#include "vertex_buffer.h"
#include "shader.h"
#include "vertex_module.h"
#include "fragment_module.h"

namespace Zafkiel
{

struct GraphicsShaderReflection
{
    VertexBufferLayout vertexInput;
};

class GraphicsShaderBackend
{
  public:
    virtual ~GraphicsShaderBackend() = default;
};

class GraphicsShader final : public Shader
{
  public:
    GraphicsShader(Scope<ShaderBackend> shaderBackend, Scope<GraphicsShaderBackend> graphicsShaderBackend)
        : Shader(std::move(shaderBackend)), backend(std::move(graphicsShaderBackend)) {}

    virtual ShaderType GetShaderType() const override { return ShaderType::Graphics; }
    
    const Observer<GraphicsShaderBackend> GetGraphicsShaderBackend() const { return backend; }
    
    const GraphicsShaderReflection &GetReflection() const { return reflection; }
    GraphicsShaderReflection &GetReflection() { return reflection; }

    const Observer<VertexModule> GetVertexModule() const { return vertexModule; }

    const Observer<FragmentModule> GetFragmentModule() const { return fragmentModule; }
    
    template<typename Derived>
    friend class GraphicsShaderFactory;
    
  protected:
    void CombineModules();

    GraphicsShaderReflection reflection;
    Scope<VertexModule> vertexModule;
    Scope<FragmentModule> fragmentModule;
    Scope<GraphicsShaderBackend> backend;
};

template<typename Derived>
class GraphicsShaderFactory
{
  protected:
    static Scope<VertexModule> &AccessVertexModule(const Scope<GraphicsShader> &shader)
    {
        return shader->vertexModule;
    }
    static Scope<FragmentModule> &AccessFragmentModule(const Scope<GraphicsShader> &shader)
    {
        return shader->fragmentModule;
    }
    static void CombineModules(const Scope<GraphicsShader> &shader)
    {
        return shader->CombineModules();
    }
};
}
