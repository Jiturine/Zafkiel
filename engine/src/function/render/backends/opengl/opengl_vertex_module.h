#pragma once
#include "opengl_shader_module.h"
#include "function/render/vertex_module.h"

namespace Zafkiel
{

class OpenGLVertexModuleBackend final : public VertexModuleBackend
{
  public:
    OpenGLVertexModuleBackend(Buffer codeBuffer, const Scope<OpenGLShaderModuleBackend> &backend);
};

class OpenGLVertexModuleFactory
{
  public:
    static Scope<VertexModule> Create(Buffer buffer);
};

}
