#include "opengl_render_resource.h"
#include "opengl_uniform_buffer.h"
#include "opengl_shader.h"
#include "function/render/graphics_shader.h"
#include <glad/glad.h>

namespace Zafkiel
{

OpenGLRenderResourceBackend::OpenGLRenderResourceBackend(const Observer<RenderResourceTemplate> renderResourceTemplate)
{
    for (auto &[resourceName, resourceType] : renderResourceTemplate->GetSchema()->GetParameterTypes())
    {
        uint32_t binding = renderResourceTemplate->GetSchema()->GetParameterBinding(resourceName);
        if (resourceType->GetCategory() == ShaderReflection::ResourceTypeCategory::UniformBlock)
        {
            uint32_t size = resourceType->As<ShaderReflection::UniformBlock>()->GetLayout().size;
            uniformBuffers[binding] = OpenGLUniformBufferFactory::Create(size);
        }
    }
}
void OpenGLRenderResourceBackend::SetTexture2D(const std::string &paramName, Observer<Texture2D> tex)
{
    uint32_t binding = renderResource->GetParameterBinding(paramName);
    sampledImages[binding] = tex;
}
void OpenGLRenderResourceBackend::SetDirty(const std::string &key) {}

void OpenGLRenderResourceBackend::AttachRenderResource(Observer<RenderResource> renderResource)
{
    this->renderResource = renderResource;
}

void OpenGLRenderResourceBackend::UploadUniform() const
{
    for (auto &[paramName, param] : renderResource->GetParameters())
    {
        if (param.type->GetCategory() != ShaderReflection::ResourceTypeCategory::UniformBlock) continue;

        uint32_t binding = renderResource->GetParameterBinding(paramName);
        auto uniformBufferBackend = uniformBuffers.at(binding)->GetBackend().As<OpenGLUniformBufferBackend>();
        auto src = param.uniformBuffer.Data<uint8_t>();
        auto size = param.uniformBuffer.Size<uint8_t>();
        uniformBufferBackend->SetData(0, size, src);
    }
}

Scope<RenderResource> OpenGLRenderResourceFactory::Create(const Observer<RenderResourceTemplate> renderResourceTemplate)
{
    auto backend = CreateScope<OpenGLRenderResourceBackend>(renderResourceTemplate);
    auto renderResource = CreateScope<RenderResource>(renderResourceTemplate, std::move(backend));
    renderResource->GetBackend().As<OpenGLRenderResourceBackend>()->AttachRenderResource(renderResource);
    return renderResource;
}
    
}