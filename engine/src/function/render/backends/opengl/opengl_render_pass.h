#pragma once
#include "function/render/render_pass.h"

namespace Zafkiel
{

class OpenGLRenderPassBackend final : public RenderPassBackend
{
  public:
    OpenGLRenderPassBackend(const RenderPassSpecification &spec);
};

class OpenGLRenderPassFactory final
{
  public:
    static Scope<RenderPass> Create(const RenderPassSpecification &spec);
};

}
