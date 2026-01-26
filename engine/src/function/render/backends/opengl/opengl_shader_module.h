#pragma once
#include "function/render/shader_module.h"

namespace Zafkiel
{

class OpenGLShaderModuleBackend final : public ShaderModuleBackend
{
  public:
    OpenGLShaderModuleBackend(uint32_t rendererID) : rendererID(rendererID) {}
    ~OpenGLShaderModuleBackend();

    uint32_t GetRendererID() const { return rendererID; }

  private:
    uint32_t rendererID;
};

}
