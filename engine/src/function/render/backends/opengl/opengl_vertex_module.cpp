#include "opengl_vertex_module.h"
#include <glad/glad.h>

namespace Zafkiel
{

OpenGLVertexModuleBackend::OpenGLVertexModuleBackend(Buffer codeBuffer, const Scope<OpenGLShaderModuleBackend> &backend)
{
    // 创建着色器对象
    backend->rendererID = glCreateShader(GL_VERTEX_SHADER);

    glShaderBinary(1, &backend->rendererID, GL_SHADER_BINARY_FORMAT_SPIR_V,
                   codeBuffer.Data<uint8_t>(), codeBuffer.Size<uint8_t>());

    glSpecializeShader(backend->rendererID, "main", 0, nullptr, nullptr);

    // 检查编译状态
    GLint compileStatus = 0;
    glGetShaderiv(backend->rendererID, GL_COMPILE_STATUS, &compileStatus);
    if (compileStatus != GL_TRUE) {
        GLint logLength = 0;
        glGetShaderiv(backend->rendererID, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0) {
            std::vector<char> log(logLength);
            glGetShaderInfoLog(backend->rendererID, logLength, nullptr, log.data());
            Log::Error("SPIR-V vertex shader specialization failed: {}", log.data());
        } else {
            Log::Error("SPIR-V vertex shader specialization failed (no error log available)");
        }
        glDeleteShader(backend->rendererID);
        backend->rendererID = 0;
        return;
    }
}

Scope<VertexModule> OpenGLVertexModuleFactory::Create(Buffer buffer)
{
    auto shaderModuleBackend = CreateScope<OpenGLShaderModuleBackend>();
    auto vertexModuleBackend = CreateScope<OpenGLVertexModuleBackend>(buffer, shaderModuleBackend);
    return CreateScope<VertexModule>(buffer, std::move(shaderModuleBackend), std::move(vertexModuleBackend));
}

}