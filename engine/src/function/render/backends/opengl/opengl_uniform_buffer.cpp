#include "function/render/backends/opengl/opengl_uniform_buffer.h"
#include <glad/glad.h>

namespace Zafkiel
{

void OpenGLUniformBufferBackend::SetData(uint32_t offset, uint32_t size, const void *data) const
{
    glBindBuffer(GL_UNIFORM_BUFFER, rendererID);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
}

OpenGLUniformBufferBackend::~OpenGLUniformBufferBackend()
{
    glDeleteBuffers(1, &rendererID);
}

}