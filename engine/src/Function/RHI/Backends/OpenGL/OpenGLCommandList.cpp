#include "Function/RHI/Backends/OpenGL/OpenGLCommandList.h"
#include "Function/RHI/Backends/OpenGL/OpenGLBuffer.h"
#include "Function/RHI/Backends/OpenGL/OpenGLPipeline.h"
#include "Function/RHI/Backends/OpenGL/OpenGLRHI.h"
#include <glad/glad.h>

namespace Zafkiel
{

static void ClearColor(uint32 attachmentIndex, ImageFormat format, ClearValue value)
{
    switch (format)
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
    case R32F:
        glClearBufferfv(GL_COLOR, attachmentIndex, &value.floatValue);
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

void OpenGLGraphicsContext::WriteBuffer(RHIBuffer *buffer, Buffer data)
{
    auto glBuffer = static_cast<OpenGLBuffer *>(buffer);

    // 使用DSA方法避免buffer绑定状态问题
    glNamedBufferSubData(glBuffer->GetHandle(), 0, data.Size<uint8>(), data.Data<uint8>());
}

void OpenGLGraphicsContext::UpdateUniformBuffer(RHIBuffer *uniformBuffer, Buffer data)
{
    auto glUniformBuffer = static_cast<OpenGLBuffer *>(uniformBuffer);

    // 使用DSA方法避免buffer绑定状态问题
    glNamedBufferSubData(glUniformBuffer->GetHandle(), 0, data.Size<uint8>(), data.Data<uint8>());
}

void OpenGLGraphicsContext::BeginRenderPass(const RHIRenderPassInfo &renderPassInfo)
{
    OpenGLRenderTargetInfo renderTargetInfo(renderPassInfo);
    auto frameBuffer = rhi.GetFrameBufferManager().GetOrCreateFramebuffer(renderTargetInfo, renderPassInfo);

    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer->GetHandle());
    glViewport(0, 0, frameBuffer->GetWidth(), frameBuffer->GetHeight());

    // 启用颜色写入
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    for (auto [colorAttachmentIndex, colorAttachmentInfo] : std::views::enumerate(renderPassInfo.colorAttachments))
    {
        ClearColor(colorAttachmentIndex, colorAttachmentInfo.texture->GetFormat(), colorAttachmentInfo.clearValue);
    }
    if (renderPassInfo.depthStencilAttachment.has_value())
    {
        ClearDepthStencil(renderPassInfo.depthStencilAttachment.value().clearValue);
    }

    currentFrameBuffer = frameBuffer;
}

void OpenGLGraphicsContext::EndRenderPass()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    currentFrameBuffer = nullptr;
}

void OpenGLGraphicsContext::BindGraphicsPipeline(RHIGraphicsPipeline *pipeline) 
{
    if (auto cullFace = CullModeToOpenGLType(pipeline->GetCullMode()))
    {
        glEnable(GL_CULL_FACE);
        glCullFace(cullFace);
    }
    else
    {
        glDisable(GL_CULL_FACE);
    }
    glFrontFace(FrontFaceToOpenGLType(pipeline->GetFrontFace()));

    glPolygonMode(GL_FRONT_AND_BACK, PolygonModeToOpenGLType(pipeline->GetPolygonMode()));

    // 设置深度测试
    if (pipeline->IsDepthTestOn())
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
    }

    glUseProgram(static_cast<OpenGLGraphicsPipeline *>(pipeline)->GetShaderProgram());

    currentPipeline = static_cast<OpenGLGraphicsPipeline *>(pipeline);

    ApplyStaticUniformBuffers(pipeline->GetShaders()[ShaderStage::Vertex]);
    ApplyStaticUniformBuffers(pipeline->GetShaders()[ShaderStage::Fragment]);
}

void OpenGLGraphicsContext::DrawIndexed(RHIBuffer *vertexBuffer, RHIBuffer *indexBuffer, uint32 indexCount)
{
    currentPipeline->SetPendingResources();

    auto glVertexBuffer = static_cast<OpenGLBuffer *>(vertexBuffer);
    auto glIndexBuffer = static_cast<OpenGLBuffer *>(indexBuffer);

    auto vertexShader = currentPipeline->GetShaders()[ShaderStage::Vertex];
    const auto &vertexInput = static_cast<RHIVertexShader *>(vertexShader)->GetVertexInput();

    uint32 vertexArray;
    glCreateVertexArrays(1, &vertexArray); // TODO: VAO Cache
    glBindVertexArray(vertexArray);

    // 绑定index buffer到VAO
    glVertexArrayElementBuffer(vertexArray, glIndexBuffer->GetHandle());

    // 绑定vertex buffer - 必须在VAO绑定之后，设置属性之前
    glBindBuffer(GL_ARRAY_BUFFER, glVertexBuffer->GetHandle());

    uint32 index = 0;
    for (auto &element : vertexInput.inputs)
    {
        switch (element.type)
        {
            using enum ShaderFundamentalType;
        case Float: case Float2: case Float3: case Float4:
        case Mat3: case Mat4:
            {
                glEnableVertexAttribArray(index);
                glVertexAttribPointer(
                    index,
                    ShaderDataTypeCount(element.type),
                    ShaderDataTypeToOpenGLBaseType(element.type),
                    false,
                    vertexInput.stride,
                    reinterpret_cast<const void*>(static_cast<uintptr_t>(element.offset)));
            }
            break;
        case Int: case Int2: case Int3: case Int4:
        case UInt: case UInt2: case UInt3: case UInt4:
        case Bool:
            {
                glEnableVertexAttribArray(index);
                glVertexAttribIPointer(
                    index,
                    ShaderDataTypeCount(element.type),
                    ShaderDataTypeToOpenGLBaseType(element.type),
                    vertexInput.stride,
                    reinterpret_cast<const void*>(static_cast<uintptr_t>(element.offset)));
            }
            break;

        default:
            Log::Error("Unsupported Shader Data Type!");
            break;
        }

        index++;
    }

    if (indexCount == 0)
    {
        glDrawElements(PrimitiveTopologyToOpenGLType(currentPipeline->GetPrimitiveTopology()),
            indexBuffer->GetSize() / sizeof(uint32), GL_UNSIGNED_INT, nullptr);
    }
    else
    {
        glDrawElements(PrimitiveTopologyToOpenGLType(currentPipeline->GetPrimitiveTopology()),
            indexCount, GL_UNSIGNED_INT, nullptr);
    }

    glDeleteVertexArrays(1, &vertexArray);
}

void OpenGLGraphicsContext::SetStaticUniformBuffer(const std::string &name, RHIBuffer *uniformBuffer) 
{
    staticUniformBuffers[name] = uniformBuffer;
}

void OpenGLGraphicsContext::Present()
{
    SDL_GL_SwapWindow(rhi.GetWindow());
}

void OpenGLGraphicsContext::ApplyStaticUniformBuffers(RHIShader *shader)
{
    for (auto staticUniformBufferName : shader->GetResourceTable().staticUniformBuffers)
    {
        currentPipeline->SetUniformBuffer(shader->GetShaderStage(), staticUniformBufferName, staticUniformBuffers[staticUniformBufferName]);
    }
}

}