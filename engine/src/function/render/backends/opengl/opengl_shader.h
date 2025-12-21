#pragma once
#include "function/render/shader.h"
#include "platform/filesystem/filesystem.h"

namespace Zafkiel
{
class OpenGLShaderBackend final : public ShaderBackend
{
  public:
    OpenGLShaderBackend();
    uint32_t GetRendererID() const { return rendererID; }

  private:
    uint32_t rendererID;
};
}
