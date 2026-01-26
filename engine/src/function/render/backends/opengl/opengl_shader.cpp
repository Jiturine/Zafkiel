#include "function/render/backends/opengl/opengl_shader.h"
#include "platform/filesystem/filesystem.h"
#include <glad/glad.h>

namespace Zafkiel
{
OpenGLShaderBackend::~OpenGLShaderBackend()
{
    glDeleteProgram(rendererID);
}
}