#include "opengl_uniform_buffer.h"
#include <glad/glad.h>

namespace Zafkiel
{

OpenGLUniformBufferBackend::OpenGLUniformBufferBackend(uint32_t size)
{
    glGenBuffers(1, &rendererID);
    glBindBuffer(GL_UNIFORM_BUFFER, rendererID);
    glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OpenGLUniformBufferBackend::SetData(uint32_t offset, uint32_t size, const void *data) const
{
    glBindBuffer(GL_UNIFORM_BUFFER, rendererID);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
}

OpenGLUniformBufferBackend::~OpenGLUniformBufferBackend()
{
    glDeleteBuffers(1, &rendererID);
}

Scope<UniformBuffer> OpenGLUniformBufferFactory::Create(uint32_t size)
{
    auto backend = CreateScope<OpenGLUniformBufferBackend>(size);
    return CreateScope<UniformBuffer>(size, std::move(backend));
}

}