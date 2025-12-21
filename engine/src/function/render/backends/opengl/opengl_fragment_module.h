#pragma once
#include "opengl_shader_module.h"
#include "function/render/fragment_module.h"

namespace Zafkiel
{

class OpenGLFragmentModuleBackend final : public FragmentModuleBackend
{
  public:
    OpenGLFragmentModuleBackend(Buffer codeBuffer, const Scope<OpenGLShaderModuleBackend> &backend);
};

class OpenGLFragmentModuleFactory
{
  public:
    static Scope<FragmentModule> Create(Buffer buffer);
};

}
