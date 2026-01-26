#include "function/render/backends/opengl/opengl_frame_buffer.h"
#include <glad/glad.h>

namespace Zafkiel
{

OpenGLFrameBufferBackend::~OpenGLFrameBufferBackend()
{
    glDeleteFramebuffers(1, &rendererID);
}

}
