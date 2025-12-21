#pragma once
#include "function/render/object_render_resource.h"

namespace Zafkiel 
{
    
class OpenGLObjectRenderResourceBackend final : public ObjectRenderResourceBackend
{
  public:
    OpenGLObjectRenderResourceBackend(const Observer<ObjectRenderResourceTemplate> objectRenderResourceTemplate);
    
    virtual void UploadUniform() override;

    void AttachRenderResource(const Observer<ObjectRenderResource> objectRenderResource) { this->objectRenderResource = objectRenderResource; }

    const std::unordered_map<uint32_t, Scope<UniformBuffer>> &GetUniformBuffers() const
    {
        return uniformBuffers;
    }
  private:
    std::unordered_map<uint32_t, Scope<UniformBuffer>> uniformBuffers;
    std::unordered_map<uint32_t, uint32_t> perObjectBufferSize;
    Observer<ObjectRenderResource> objectRenderResource;
};

class OpenGLObjectRenderResourceFactory
{
  public:
    static Scope<ObjectRenderResource> Create(const Path &path);
};
}