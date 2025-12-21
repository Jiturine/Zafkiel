#include "opengl_fragment_module.h"
#include <glad/glad.h>

namespace Zafkiel
{

OpenGLFragmentModuleBackend::OpenGLFragmentModuleBackend(Buffer codeBuffer, const Scope<OpenGLShaderModuleBackend> &backend)
{
    backend->rendererID = glCreateShader(GL_FRAGMENT_SHADER);

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
            Log::Error("SPIR-V fragment shader specialization failed: {}", log.data());
        } else {
            Log::Error("SPIR-V fragment shader specialization failed (no error log available)");
        }
        glDeleteShader(backend->rendererID);
        backend->rendererID = 0;
        return;
    }
}

Scope<FragmentModule> OpenGLFragmentModuleFactory::Create(Buffer buffer)
{
    auto shaderModuleBackend = CreateScope<OpenGLShaderModuleBackend>();
    auto fragmentModuleBackend = CreateScope<OpenGLFragmentModuleBackend>(buffer, shaderModuleBackend);
    return CreateScope<FragmentModule>(buffer, std::move(shaderModuleBackend), std::move(fragmentModuleBackend));
}

}
