#pragma once
#include "function/render/object_render_resource_template.h"
#include "shader.h"

namespace Zafkiel
{
static constexpr uint32_t maxObjectNum = 1024;

struct ObjectRenderResourceParameter
{
    const ShaderReflection::ResourceType *type;
    ScopedBuffer dynamicUniformBuffer = nullptr;
    
    ObjectRenderResourceParameter() = default;
    ObjectRenderResourceParameter(const ShaderReflection::UniformBlock *uniformBlock) : type(uniformBlock)
    {
        uint32_t size = uniformBlock->GetLayout().size;
        dynamicUniformBuffer = ScopedBuffer(size * maxObjectNum);
    }
};
  
class ObjectRenderResourceBackend 
{
  public:
    
    virtual ~ObjectRenderResourceBackend() = default;
    virtual void UploadUniform() = 0;
};

class ObjectRenderResource final
{
  public:
    ObjectRenderResource(Scope<ObjectRenderResourceTemplate> objectRenderResourceTemplate, Scope<ObjectRenderResourceBackend> backend)
        : objectRenderResourceTemplate(std::move(objectRenderResourceTemplate)), backend(std::move(backend)) 
    {
        for (auto &[paramName, paramType] : this->objectRenderResourceTemplate->GetParameterTypes())
        {
            if (paramType->GetCategory() == ShaderReflection::ResourceTypeCategory::UniformBlock)
            {
                parameters[paramName] = ObjectRenderResourceParameter(paramType->As<ShaderReflection::UniformBlock>());
                parameterBindings[paramName] = this->objectRenderResourceTemplate->GetParameterBinding(paramName);
            }
        }
    }

    void UploadUniform() 
    {
        backend->UploadUniform();
    }

    template <typename T>
    void SetUniform(uint32_t index, const std::string &key, T value)
    {
        auto paramPath = objectRenderResourceTemplate->GetAliasPath(key);
        SetUniform(index, paramPath, value);
    }

    template <typename T>
    void SetUniform(uint32_t index, const RenderResourceParameterPath &path, T value);

    Observer<ObjectRenderResourceBackend> GetBackend() { return backend; }
    const Observer<ObjectRenderResourceBackend> GetBackend() const { return backend; }
    
    const Observer<ObjectRenderResourceTemplate> GetTemplate() const { return objectRenderResourceTemplate; }
    Observer<ObjectRenderResourceTemplate> GetTemplate() { return objectRenderResourceTemplate; }
    uint32_t GetParameterBinding(const std::string &key) const { return parameterBindings.at(key); }

    const std::unordered_map<std::string, ObjectRenderResourceParameter> &GetParameters() const { return parameters; }

  private:
    std::unordered_map<std::string, ObjectRenderResourceParameter> parameters;
    std::unordered_map<std::string, uint32_t> parameterBindings;
    Scope<ObjectRenderResourceBackend> backend;
    Scope<ObjectRenderResourceTemplate> objectRenderResourceTemplate;
};

}

#include "object_render_resource.tpp"