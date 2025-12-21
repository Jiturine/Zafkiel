#pragma once
#include "function/render/graphics_shader.h"

namespace Zafkiel
{

class OpenGLGraphicsShaderBackend final : public GraphicsShaderBackend
{
  public:

};

class OpenGLGraphicsShaderFactory : public GraphicsShaderFactory<OpenGLGraphicsShaderFactory>
{
  public:
    static Scope<GraphicsShader> Create(const Path &path);
};


}
