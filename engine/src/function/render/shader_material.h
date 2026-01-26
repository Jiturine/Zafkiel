#pragma once
#include "function/render/shader_material_template.h"
#include "function/render/shader_family.h"
#include "function/render/shader_module.h"
#include "function/render/uniform_buffer.h"
#include "function/render/texture.h"
#include "function/render/render_registry.h"

namespace Zafkiel
{

struct ShaderResource
{
    const ShaderReflection::ResourceType *type = nullptr;
    ScopedBuffer uniformBuffer = nullptr;
    std::optional<RenderHandle> handle;

    ShaderResource(const ShaderReflection::UniformBlock *uniformBlock)
        : type(uniformBlock), uniformBuffer(ScopedBuffer(uniformBlock->GetSize())) {}
    ShaderResource(ShaderReflection::SamplerType type, RenderHandle handle) 
        : type(ShaderReflection::GetSampledImageType(type)), handle(handle) {}
    ShaderResource(ShaderReflection::SamplerType type) 
        : type(ShaderReflection::GetSampledImageType(type)) {}
};
class ShaderMaterialBackend
{
  public:
    virtual ~ShaderMaterialBackend() = default;
    virtual void SetTexture2D(uint32_t binding, RenderHandle handle) = 0;
};

class ShaderMaterial final
{
  public:
    ShaderMaterial(RenderHandle shaderMaterialSchema, std::vector<std::optional<ShaderResource>> resources, Scope<ShaderMaterialBackend> backend);

    std::vector<std::optional<ShaderResource>> &GetResources() { return resources; }
    const std::vector<std::optional<ShaderResource>> &GetResources() const { return resources; }
  
    void SetTexture2D(uint32_t binding, RenderHandle texture2D);

    RenderHandle GetSchema() const { return shaderMaterialSchema; }

    Borrow<ShaderMaterialBackend> GetBackend() const { return Borrow(backend); }
    MutBorrow<ShaderMaterialBackend> GetBackend() { return MutBorrow(backend); }

  private:
    std::vector<std::optional<ShaderResource>> resources;
    RenderHandle shaderMaterialSchema;
    Scope<ShaderMaterialBackend> backend;
};
    
}