#include "opengl_command.h"
#include "opengl_vertex_buffer.h"
#include "opengl_index_buffer.h"
#include "opengl_context.h"
#include "opengl_render_pass.h"
#include "opengl_material.h"
#include "opengl_object_render_resource.h"
#include "opengl_graphics_pipeline.h"
#include "opengl_shader.h"
#include "opengl_uniform_buffer.h"
#include "opengl_frame_buffer.h"
#include "opengl_image.h"
#include <glad/glad.h>

static constexpr uint32_t GlobalRenderResourceBindingBase = 0;
static constexpr uint32_t RenderPassResourceBindingBase = 16;
static constexpr uint32_t MaterialBindingBase = 32;
static constexpr uint32_t ObjectRenderResourceBindingBase = 48;

constexpr size_t AlignUp(size_t value, size_t alignment) 
{
    return (value + alignment - 1) & ~(alignment - 1);
}

namespace Zafkiel
{
static void ClearColor(uint32_t attachmentIndex, ClearValue value)
{
    switch (value.format)
    {
        using enum ImageFormat;
    case R8:
    case R8_sRGB:
        glClearBufferfv(GL_COLOR, attachmentIndex, &value.floatValue);
        break;
    case RG8:
    case RG8_sRGB:
        glClearBufferfv(GL_COLOR, attachmentIndex, (float*)&value.vec2Value);
        break;
    case BGR8:
    case RGB8:
    case RGB8_sRGB:
    case BGR8_sRGB:
    case RGB16F:
        glClearBufferfv(GL_COLOR, attachmentIndex, (float*)&value.vec3Value);
        break;
    case BGRA8:
    case RGBA8:
    case RGBA8_sRGB:
    case BGRA8_sRGB:
    case RGBA16F:
    case RGBA32F:
        glClearBufferfv(GL_COLOR, attachmentIndex, (float*)&value.vec4Value);
        break;
    case R32UI:
        glClearBufferuiv(GL_COLOR, attachmentIndex, &value.uintValue);
        break;
    default:
        Log::Error("Unsupported Clear Color Value!");
        break;
    }
}

static void ClearDepthStencil(ClearValue value)
{
    glClearBufferfi(GL_DEPTH_STENCIL, 0, value.floatValue, value.uintValue);
}

static void ClearDepth(ClearValue value)
{
    glClearBufferfv(GL_DEPTH, 0, &value.floatValue);
}

static void ClearStencil(ClearValue value)
{
    glClearBufferuiv(GL_STENCIL, 0, &value.uintValue);
}


void OpenGLCommand::BeginRenderPassImpl(const RenderPassBeginInfo &beginInfo)
{
    auto frameBuffer = beginInfo.frameBuffer;
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer->GetBackend().As<OpenGLFrameBufferBackend>()->GetRendererID());
    glViewport(0, 0, frameBuffer->GetWidth(), frameBuffer->GetHeight());
    
    uint32_t colorAttachmentIndex = 0;
    for (auto &clearValue : beginInfo.clearValues)
    {
        if (clearValue.type == AttachmentType::Color)
        {
            ClearColor(colorAttachmentIndex, clearValue);
            colorAttachmentIndex++;
        }
        else if (clearValue.type == AttachmentType::DepthStencil)
            ClearDepthStencil(clearValue);
        else if (clearValue.type == AttachmentType::Depth)
            ClearDepth(clearValue);
        else if (clearValue.type == AttachmentType::Stencil)
            ClearStencil(clearValue);
    }
    
    currentFrameBuffer = beginInfo.frameBuffer;
}
void OpenGLCommand::EndRenderPassImpl()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    currentFrameBuffer = nullptr;
}
void OpenGLCommand::BindPipelineImpl(const Observer<Pipeline> pipeline)
{
    if (pipeline->GetPipelineType() == PipelineType::Graphics)
    {
        auto graphicsPipeline = pipeline.As<GraphicsPipeline>();
        auto backend = graphicsPipeline->GetGraphicsPipelineBackend().As<OpenGLGraphicsPipelineBackend>();

        if (backend->GetCullMode())
        {
            glEnable(GL_CULL_FACE);
            glCullFace(backend->GetCullMode());
        }
        else
        {
            glDisable(GL_CULL_FACE);
        }
        glFrontFace(backend->GetFrontFace());

        glPolygonMode(GL_FRONT_AND_BACK, backend->GetPolygonMode());

        // 设置深度测试
        if (backend->GetDepthTest())
        {
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
        }
        else
        {
            glDisable(GL_DEPTH_TEST);
        }

        auto shaderBackend = graphicsPipeline->GetShader()->GetShaderBackend().As<OpenGLShaderBackend>();
        glUseProgram(shaderBackend->GetRendererID());
    }

    currentPipeline = pipeline;
}
/* 
void OpenGLCommand::BindRenderResourceImpl(const Observer<RenderResource> renderResource)
{
    auto backend = renderResource->GetBackend().As<OpenGLRenderResourceBackend>();
    backend->UploadUniform();

    for (auto &[binding, uniformBuffer] : backend->GetUniformBuffers())
    {
        glBindBufferBase(GL_UNIFORM_BUFFER, binding, uniformBuffer->GetBackend().As<OpenGLUniformBufferBackend>()->GetRendererID());
    }
    for (auto &[binding, sampledImage] : backend->GetSampledImages())
    {
        glBindTextureUnit(binding, sampledImage->GetImage()->GetBackend().As<OpenGLImageBackend>()->GetRendererID());
    }
} */
void OpenGLCommand::DrawIndexedImpl(const Observer<VertexBuffer> vertexBuffer, const Observer<IndexBuffer> indexBuffer)
{
    uint32_t vertexArray;
    glCreateVertexArrays(1, &vertexArray); // TODO: VAO Cache
    glBindVertexArray(vertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer->GetBackend().As<OpenGLVertexBufferBackend>()->GetRendererID());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer->GetBackend().As<OpenGLIndexBufferBackend>()->GetRendererID());

    uint32_t index = 0;
    const auto &layout = currentPipeline.As<GraphicsPipeline>()->GetShader()->GetReflection().vertexInput;

    for (auto element : layout.elements)
    {
        switch (element.type)
        {
            using enum ShaderFundamentalType;
        case Float:
        case Float2:
        case Float3:
        case Float4:
        case Mat3:
        case Mat4:
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(
				index,
				ShaderDataTypeCount(element.type),
				ShaderDataTypeToOpenGLBaseType(element.type),
                false,
				layout.stride,
				reinterpret_cast<const void *>(element.offset));
            break;

        case Int:
        case Int2:
        case Int3:
        case Int4:
        case UInt:
        case UInt2:
        case UInt3:
        case UInt4:
        case Bool:
			glEnableVertexAttribArray(index);
			glVertexAttribIPointer(
				index,
				ShaderDataTypeCount(element.type),
				ShaderDataTypeToOpenGLBaseType(element.type),
				layout.stride,
				reinterpret_cast<const void *>(element.offset));
            break;

        default:
            Log::Error("Unsupported Shader Data Type!");
            break;
        }

        index++;
    }
    glDrawElements(currentPipeline.As<GraphicsPipeline>()->GetGraphicsPipelineBackend().As<OpenGLGraphicsPipelineBackend>()->GetPrimitiveTopology(),
        indexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr);
    
    glDeleteVertexArrays(1, &vertexArray);
}
void OpenGLCommand::BeginFrame(const Scope<GraphicsContext> &context)
{

}

void OpenGLCommand::BindGlobalRenderResourceImpl(const Observer<GlobalRenderResource> globalRenderResource)
{
    auto backend = globalRenderResource->GetRenderResource()->GetBackend().As<OpenGLRenderResourceBackend>();
    backend->UploadUniform();

    for (auto &[binding, uniformBuffer] : backend->GetUniformBuffers())
    {
        glBindBufferBase(GL_UNIFORM_BUFFER, GlobalRenderResourceBindingBase + binding, uniformBuffer->GetBackend().As<OpenGLUniformBufferBackend>()->GetRendererID());
    }
    for (auto &[binding, sampledImage] : backend->GetSampledImages())
    {
        glBindTextureUnit(GlobalRenderResourceBindingBase + binding, sampledImage->GetImage()->GetBackend().As<OpenGLImageBackend>()->GetRendererID());
    }
}
void OpenGLCommand::BindRenderPassResourceImpl(const Observer<RenderPassResource> renderPassResource)
{
    auto backend = renderPassResource->GetRenderResource()->GetBackend().As<OpenGLRenderResourceBackend>();
    backend->UploadUniform();

    for (auto &[binding, uniformBuffer] : backend->GetUniformBuffers())
    {
        glBindBufferBase(GL_UNIFORM_BUFFER, RenderPassResourceBindingBase + binding, uniformBuffer->GetBackend().As<OpenGLUniformBufferBackend>()->GetRendererID());
    }
    for (auto &[binding, sampledImage] : backend->GetSampledImages())
    {
        glBindTextureUnit(RenderPassResourceBindingBase + binding, sampledImage->GetImage()->GetBackend().As<OpenGLImageBackend>()->GetRendererID());
    }
}
void OpenGLCommand::BindMaterialImpl(const Observer<Material> material)
{
    auto backend = material->GetRenderResource()->GetBackend().As<OpenGLRenderResourceBackend>();
    backend->UploadUniform();

    for (auto &[binding, uniformBuffer] : backend->GetUniformBuffers())
    {
        glBindBufferBase(GL_UNIFORM_BUFFER, MaterialBindingBase + binding, uniformBuffer->GetBackend().As<OpenGLUniformBufferBackend>()->GetRendererID());
    }
    for (auto &[binding, sampledImage] : backend->GetSampledImages())
    {
        glBindTextureUnit(MaterialBindingBase + binding, sampledImage->GetImage()->GetBackend().As<OpenGLImageBackend>()->GetRendererID());
    }
}
void OpenGLCommand::BindObjectRenderResourceImpl(uint32_t index, const Observer<ObjectRenderResource> objectRenderResource)
{
    auto backend = objectRenderResource->GetBackend().As<OpenGLObjectRenderResourceBackend>();
    int alignment;
    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &alignment);

    for (auto &[paramName, paramType] : objectRenderResource->GetTemplate()->GetParameterTypes())
    {
        if (paramType->GetCategory() == ShaderReflection::ResourceTypeCategory::UniformBlock)
        {
            auto uniformBufferType = paramType->As<ShaderReflection::UniformBlock>();
            uint32_t perObjectsize = uniformBufferType->GetLayout().size;
            uint32_t stride = AlignUp(perObjectsize, alignment);
            uint32_t binding = objectRenderResource->GetTemplate()->GetParameterBinding(paramName);
            auto uniformBufferRendererID = objectRenderResource->GetBackend().As<OpenGLObjectRenderResourceBackend>()->GetUniformBuffers().at(binding)->GetBackend().As<OpenGLUniformBufferBackend>()->GetRendererID();

            glBindBufferRange(GL_UNIFORM_BUFFER, ObjectRenderResourceBindingBase + binding, uniformBufferRendererID,
            index * perObjectsize, perObjectsize);
        }
    }
}

}
