#pragma once
#include "function/render/render_resource.h"
#include "function/render/render_resource_template.h"
#include "function/render/uniform_buffer.h"

namespace Zafkiel
{

class OpenGLRenderResourceBackend final : public RenderResourceBackend
{
  public:
    OpenGLRenderResourceBackend(const Observer<RenderResourceTemplate> renderResourceTemplate);
    virtual void SetTexture2D(const std::string &paramName, Observer<Texture2D> tex) override;
    virtual void SetDirty(const std::string &key) override;
    void UploadUniform() const;
    const std::unordered_map<uint32_t, Scope<UniformBuffer>> &GetUniformBuffers() const
    {
        return uniformBuffers;
    }
    const std::unordered_map<uint32_t, Observer<Texture2D>> &GetSampledImages() const
    {
        return sampledImages;
    }
    
    friend class OpenGLRenderResourceFactory;
    
  private:
    void AttachRenderResource(Observer<RenderResource> renderResource);

    Observer<RenderResource> renderResource;
    Observer<Shader> shader;

    std::unordered_map<uint32_t, Scope<UniformBuffer>> uniformBuffers;

    std::unordered_map<uint32_t, Observer<Texture2D>> sampledImages;
};

class OpenGLRenderResourceFactory
{
  public:
    static Scope<RenderResource> Create(const Observer<RenderResourceTemplate> renderResourceTemplate);
};

}
