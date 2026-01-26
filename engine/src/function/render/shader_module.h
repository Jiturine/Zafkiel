#pragma once
#include "function/render/uniform_buffer.h"
#include "function/render/shader.h"
#include "function/render/render_handle.h"

namespace Zafkiel
{

enum class ShaderStage
{
    None = 0,
    Vertex,
    Fragment,
    Geometry
};

struct SampledImageLayout
{
    std::string name;
    uint32_t binding;
};

struct ShaderModuleReflection
{
    struct InputLayout
    {
        std::string name;
        uint32_t location;
        uint32_t size;
        uint32_t offset;
        ShaderFundamentalType type;
    };
    // std::vector<UniformBufferLayout> uniformBuffers;
    // std::vector<SampledImageLayout> sampledImages;
    std::vector<InputLayout> inputs;
};

class ShaderModuleBackend
{
  public:
    virtual ~ShaderModuleBackend() = default;
};

class ShaderModule
{
  public:
    virtual ~ShaderModule() = default;
    virtual ShaderStage GetShaderStage() const = 0;

    ShaderModule(Buffer codeBuffer, Scope<ShaderModuleBackend> backend)
        : backend(std::move(backend))
    {
        Reflect(codeBuffer);
    }

    Borrow<ShaderModuleBackend> GetBackend() const { return Borrow(backend); }

    ShaderModuleReflection &GetReflection() { return reflection; }
    const ShaderModuleReflection &GetReflection() const { return reflection; }

  protected:
    void Reflect(Buffer codeBuffer);
    ShaderModuleReflection reflection;
    Scope<ShaderModuleBackend> backend;
};

}
