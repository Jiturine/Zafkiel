#include "function/render/backends/opengl/opengl_shader_module.h"
#include <glad/glad.h>

namespace Zafkiel 
{

OpenGLShaderModuleBackend::~OpenGLShaderModuleBackend()
{
    glDeleteShader(rendererID);
}
  
}
