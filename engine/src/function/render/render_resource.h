#pragma once
#include "render_resource_template.h"
#include "shader_family.h"
#include "shader_module.h"
#include "uniform_buffer.h"
#include "texture.h"

namespace Zafkiel
{

struct RenderResourceParameter
{
    const ShaderReflection::ResourceType *type;
    ScopedBuffer uniformBuffer = nullptr;
    Observer<Texture2D> texture2D = nullptr;
    
    RenderResourceParameter() = default;
    RenderResourceParameter(const ShaderReflection::UniformBlock *uniformBlock) : type(uniformBlock), uniformBuffer(ScopedBuffer(uniformBlock->GetLayout().size)) {}
    RenderResourceParameter(Observer<Texture2D> v) : type(ShaderReflection::GetSampledImageType(ShaderReflection::SamplerType::Texture2D)), texture2D(v) {}
    RenderResourceParameter(ShaderReflection::SamplerType type) : type(ShaderReflection::GetSampledImageType(type)) {}
};
class RenderResourceBackend
{
  public:
    virtual ~RenderResourceBackend() = default;
    virtual void SetTexture2D(const std::string &paramName, Observer<Texture2D> tex) = 0;
    virtual void SetDirty(const std::string &key) = 0;
};

class RenderResource final
{
  public:
    RenderResource(const Observer<RenderResourceTemplate> renderResourceTemplate, Scope<RenderResourceBackend> backend);

    std::unordered_map<std::string, RenderResourceParameter> &GetParameters() { return parameters; }
    const std::unordered_map<std::string, RenderResourceParameter> &GetParameters() const { return parameters; }

    template <typename T>
    void SetUniform(const std::string &key, T value)
    {
        auto paramPath = renderResourceTemplate->GetSchema()->GetAliasPath(key);
        SetUniform(paramPath, value);
    }

    template <typename T>
    T GetUniform(const std::string &key)
    {
        auto paramPath = renderResourceTemplate->GetSchema()->GetAliasPath(key);
        return GetUniform<T>(paramPath);
    }

    template <typename T>
    T GetUniform(const RenderResourceParameterPath &path);

    template <typename T>
    void SetUniform(const RenderResourceParameterPath &path, T value);

    void SetTexture2D(const std::string &key, Observer<Texture2D> tex);

    Observer<Texture2D> GetTexture2D(const std::string &key) const;

    bool HasTexture2D(const std::string &key) const { return parameters.contains(key); }

    void SetDirty(const std::string &key) { return backend->SetDirty(key); }

    Observer<RenderResourceBackend> GetBackend() { return backend; }
    const Observer<RenderResourceBackend> GetBackend() const { return backend; }
    
    const Observer<RenderResourceTemplate> GetTemplate() const { return renderResourceTemplate; }

    uint32_t GetParameterBinding(const std::string &key) const { return parameterBindings.at(key); }

  private:
    std::unordered_map<std::string, RenderResourceParameter> parameters;
    std::unordered_map<std::string, uint32_t> parameterBindings;
    const Observer<RenderResourceTemplate> renderResourceTemplate;
    Scope<RenderResourceBackend> backend;
};
    
}

#include "render_resource.tpp"