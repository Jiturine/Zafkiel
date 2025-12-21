#include "opengl_object_render_resource.h"
#include "opengl_object_render_resource_template.h"
#include "opengl_uniform_buffer.h"
#include <glad/glad.h>

namespace Zafkiel 
{

constexpr size_t AlignUp(size_t value, size_t alignment) 
{
    return (value + alignment - 1) & ~(alignment - 1);
}

OpenGLObjectRenderResourceBackend::OpenGLObjectRenderResourceBackend(const Observer<ObjectRenderResourceTemplate> renderResourceTemplate)
{
    int alignment;
    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &alignment);
    for (auto &[resourceName, resourceType] : renderResourceTemplate->GetParameterTypes())
    {
        uint32_t binding = renderResourceTemplate->GetParameterBinding(resourceName);
        if (resourceType->GetCategory() == ShaderReflection::ResourceTypeCategory::UniformBlock)
        {
            auto uniformBufferType = resourceType->As<ShaderReflection::UniformBlock>();
            uint32_t perObjectsize = uniformBufferType->GetLayout().size;
            uint32_t stride = AlignUp(perObjectsize, alignment);
            uint32_t bufferSize = stride * maxObjectNum;
            uniformBuffers[binding] = OpenGLUniformBufferFactory::Create(bufferSize);
        }
    }
}

void OpenGLObjectRenderResourceBackend::UploadUniform()
{
    int alignment;
    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &alignment);
    for (auto &[paramName, param] : objectRenderResource->GetParameters())
    {
        if (param.type->GetCategory() != ShaderReflection::ResourceTypeCategory::UniformBlock) continue;

        uint32_t binding = objectRenderResource->GetParameterBinding(paramName);
        auto uniformBufferBackend = uniformBuffers.at(binding)->GetBackend().As<OpenGLUniformBufferBackend>();
        auto uniformBufferType = param.type->As<ShaderReflection::UniformBlock>();
        uint32_t perObjectsize = uniformBufferType->GetLayout().size;
        uint32_t stride = AlignUp(perObjectsize, alignment);
        for (uint32_t i = 0; i < maxObjectNum; i++)
        {
            auto src = param.dynamicUniformBuffer.Data<uint8_t>() + i * perObjectsize;
            uniformBufferBackend->SetData(i * stride, perObjectsize, src);
        }
    }
}

Scope<ObjectRenderResource> OpenGLObjectRenderResourceFactory::Create(const Path &path)
{
    auto renderResourceTemplate = OpenGLObjectRenderResourceTemplateFactory::Create(path);
    auto backend = CreateScope<OpenGLObjectRenderResourceBackend>(renderResourceTemplate);
    auto result = CreateScope<ObjectRenderResource>(std::move(renderResourceTemplate), std::move(backend));
    result->GetBackend().As<OpenGLObjectRenderResourceBackend>()->AttachRenderResource(result);
    return result;
}

}