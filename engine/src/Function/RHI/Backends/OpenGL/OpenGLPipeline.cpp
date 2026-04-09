#include "Function/RHI/Backends/OpenGL/OpenGLPipeline.h"
#include "Function/RHI/Backends/OpenGL/OpenGLBuffer.h"
#include "Function/RHI/Backends/OpenGL/OpenGLShader.h"
#include "Function/RHI/Backends/OpenGL/OpenGLTexture.h"

namespace Zafkiel
{

GLenum PrimitiveTopologyToOpenGLType(PrimitiveTopology type)
{
    switch (type)
    {
        using enum PrimitiveTopology;
    case Triangles: return GL_TRIANGLES;
    case TriangleStrip: return GL_TRIANGLE_STRIP;
    case TriangleFan: return GL_TRIANGLE_FAN;
    default:
        Log::Error("Unknown Primitive Topology!");
        return GL_NONE;
    }
}
GLenum CullModeToOpenGLType(CullMode mode)
{
    switch (mode)
    {
        using enum CullMode;
    case Front: return GL_FRONT;
    case Back: return GL_BACK;
    // TODO: FrontAndBack
    default:
        return GL_NONE;
    }
}
GLenum FrontFaceToOpenGLType(FrontFace type)
{
    switch (type)
    {
        using enum FrontFace;
    case CounterClockWise: return GL_CCW;
    case ClockWise: return GL_CW;
    default:
        return GL_CCW;
    }
}
GLenum PolygonModeToOpenGLType(PolygonMode mode)
{
    switch (mode)
    {
        using enum PolygonMode;
    case Fill: return GL_FILL;
    case Wireframe: return GL_LINE;
    default:
        Log::Error("Unsupported Polygon Mode!");
        return GL_FILL;
    }
}

OpenGLGraphicsPipeline::OpenGLGraphicsPipeline(const RHIGraphicsPipelineDesc &desc)
    : RHIGraphicsPipeline(desc)
{
    shaderProgram = glCreateProgram();
    auto vertexShader = static_cast<OpenGLVertexShader *>(desc.shaders[ShaderStage::Vertex]);
    auto fragmentShader = static_cast<OpenGLFragmentShader *>(desc.shaders[ShaderStage::Fragment]);

    pendingResources[ShaderStage::Vertex] = OpenGLPendingPipelineResources();
    pendingResources[ShaderStage::Fragment] = OpenGLPendingPipelineResources();

    glAttachShader(shaderProgram, vertexShader->GetHandle());
    glAttachShader(shaderProgram, fragmentShader->GetHandle());

    glLinkProgram(shaderProgram);

    // 检查链接状态
    GLint linkStatus = 0;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linkStatus);
    if (linkStatus != GL_TRUE) 
    {
        GLint logLength = 0;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0) 
        {
            std::vector<char> log(logLength);
            glGetProgramInfoLog(shaderProgram, logLength, nullptr, log.data());
            Log::Error("Shader program linking failed: {}", log.data());
        } 
        else 
        {
            Log::Error("Shader program linking failed (no error log available)");
        }
    }
}

void OpenGLGraphicsPipeline::SetPendingResources()
{
    int alignment;
    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &alignment);

    for (auto &[stage, pendingResource] : pendingResources)
    {
        auto &SRT = desc.shaders[stage]->GetResourceTable();

        uint32 bindingBase = (stage == ShaderStage::Vertex) ? 0 :
                            (stage == ShaderStage::Fragment) ? 16 :
                            32;

        for (auto [binding, uniformBuffer] : pendingResource.uniformBuffers)
        {
            if (pendingResource.dynamicUniformBufferIndices.contains(binding))
            {
                auto uniformBlock = SRT.resourceTypeInfos[binding].type->As<ShaderReflection::UniformBlock>();
                uint32 stride = AlignUp(uniformBlock->GetSize(), alignment);
                uint32 index = pendingResource.dynamicUniformBufferIndices[binding].value();
                uint32 offset = stride * index;

                glBindBufferRange(GL_UNIFORM_BUFFER, bindingBase + binding, uniformBuffer->GetHandle(), offset, uniformBlock->GetSize());
            }
            else
            {
                glBindBufferBase(GL_UNIFORM_BUFFER, bindingBase + binding, uniformBuffer->GetHandle());
            }
        }

        for (auto [binding, texture] : pendingResource.textures)
        {
            uint32 textureUnit = bindingBase + binding;
            GLuint handle = static_cast<OpenGLTexture*>(texture)->GetHandle();
            glBindTextureUnit(textureUnit, handle);
        }
    }
}

void OpenGLGraphicsPipeline::SetDynamicOffsetIndex(ShaderStage::Stage stage, const std::string &name, uint32 index) 
{
    auto &SRT = desc.shaders[stage]->GetResourceTable();
    
    uint32 binding = SRT.resourceNameToBinding[name];

    pendingResources[stage].dynamicUniformBufferIndices[binding] = index;
}

void OpenGLGraphicsPipeline::SetUniformBuffer(ShaderStage::Stage stage, const std::string &name, RHIBuffer *buffer)
{
    auto &SRT = desc.shaders[stage]->GetResourceTable();
    
    uint32 binding = SRT.resourceNameToBinding[name];

    pendingResources[stage].uniformBuffers[binding] = static_cast<OpenGLBuffer *>(buffer);
}

void OpenGLGraphicsPipeline::SetTexture(ShaderStage::Stage stage, const std::string &name, RHITexture *texture) 
{
    auto &SRT = desc.shaders[stage]->GetResourceTable();
    
    uint32 binding = SRT.resourceNameToBinding[name];

    pendingResources[stage].textures[binding] = static_cast<OpenGLTexture *>(texture);
}

void OpenGLGraphicsPipeline::ClearResources() 
{
    pendingResources.clear();
}

}
