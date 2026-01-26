#pragma once
#include "function/render/object_shader_material_template.h"
#include "function/render/render_registry.h"
#include "function/render/shader.h"

namespace Zafkiel
{
static constexpr uint32_t maxObjectNum = 1024;

struct ObjectShaderResource
{
    const ShaderReflection::ResourceType *type;
    ScopedBuffer dynamicUniformBuffer;
    
    ObjectShaderResource(const ShaderReflection::UniformBlock *uniformBlock) : type(uniformBlock)
    {
        uint32_t size = uniformBlock->GetSize();
        dynamicUniformBuffer = ScopedBuffer(size * maxObjectNum);
    }
};
  
class ObjectShaderMaterialBackend 
{
  public:
    virtual ~ObjectShaderMaterialBackend() = default;
};

class ObjectShaderMaterial final
{
  public:
    ObjectShaderMaterial(RenderHandle shaderMaterialSchema, RenderHandle objectShaderMaterialTemplate, std::vector<std::optional<ObjectShaderResource>> resources, Scope<ObjectShaderMaterialBackend> backend);

    Borrow<ObjectShaderMaterialBackend> GetBackend() const { return Borrow(backend); }
    MutBorrow<ObjectShaderMaterialBackend> GetBackend()  { return MutBorrow(backend); }
    
    RenderHandle GetSchema() const { return shaderMaterialSchema; }
    RenderHandle GetTemplate() const { return objectShaderMaterialTemplate; }

    const std::vector<std::optional<ObjectShaderResource>> &GetResources() const { return resources; }
    std::vector<std::optional<ObjectShaderResource>> &GetResources() { return resources; }

  private:
    std::vector<std::optional<ObjectShaderResource>> resources;
    RenderHandle shaderMaterialSchema;
    RenderHandle objectShaderMaterialTemplate;
    Scope<ObjectShaderMaterialBackend> backend;
};

}