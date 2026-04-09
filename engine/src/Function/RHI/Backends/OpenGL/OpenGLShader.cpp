#include "Function/RHI/Backends/OpenGL/OpenGLShader.h"
#include "Function/RHI/Backends/Vulkan/VulkanShader.h"
#include "Platform/Filesystem/Filesystem.h"
#include <glad/glad.h>
#include <spirv_cross/spirv_cross.hpp>

namespace Zafkiel
{

OpenGLShader::OpenGLShader(Buffer code, ShaderType shaderType, ShaderResourceTable &shaderResourceTable)
    : SRT(shaderResourceTable)
{
    GLenum glShaderType = shaderType == ShaderType::Vertex ? GL_VERTEX_SHADER :
                          shaderType == ShaderType::Fragment ? GL_FRAGMENT_SHADER :
                                                               GL_GEOMETRY_SHADER;

    handle = glCreateShader(glShaderType);

    glShaderBinary(1, &handle, GL_SHADER_BINARY_FORMAT_SPIR_V, code.Data<uint8>(), code.Size<uint8>());

    glSpecializeShader(handle, "main", 0, nullptr, nullptr);

    // 检查编译状态
    GLint compileStatus = 0;
    glGetShaderiv(handle, GL_COMPILE_STATUS, &compileStatus);
    if (compileStatus != GL_TRUE) 
    {
        GLint logLength = 0;
        glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0) 
        {
            std::vector<char> log(logLength);
            glGetShaderInfoLog(handle, logLength, nullptr, log.data());
            Log::Error("SPIR-V shader specialization failed: {}", log.data());
        } 
        else 
        {
            Log::Error("SPIR-V shader specialization failed (no error log available)");
        }
        glDeleteShader(handle);
    }
}

OpenGLShader::~OpenGLShader()
{
    glDeleteShader(handle);
}

OpenGLVertexShader::OpenGLVertexShader(Buffer code, Scope<ShaderResourceTable> SRT)
    : RHIVertexShader(MoveTemp(SRT)),
      OpenGLShader(code, ShaderType::Vertex, *shaderResourceTable.get())
{
    spirv_cross::Compiler compiler(code.Data<uint32>(), code.Size<uint32>());
    spirv_cross::ShaderResources resources = compiler.get_shader_resources();

    uint32 offset = 0;
    for (const auto &input : resources.stage_inputs)
    {
        auto &type = compiler.get_type(input.type_id);
        auto dataType = SPIRTypeToShaderDataType(type)->GetKind();
        ShaderVertexInput::InputLayout elementLayout
        {
            .name = compiler.get_name(input.id),
            .location = compiler.get_decoration(input.id, spv::DecorationLocation),
            .size = ShaderDataTypeSize(dataType),
            .offset = offset,
            .type = dataType
        };
        vertexInput.inputs.push_back(elementLayout);
        offset += elementLayout.size;
    }
    vertexInput.stride = offset;
}

}