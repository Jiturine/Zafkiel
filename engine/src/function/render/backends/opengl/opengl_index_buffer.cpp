#include "function/render/backends/opengl/opengl_index_buffer.h"
#include <glad/glad.h>

namespace Zafkiel
{

OpenGLIndexBufferBackend::~OpenGLIndexBufferBackend()
{
    glDeleteBuffers(1, &rendererID);
}

}
