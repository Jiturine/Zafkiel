#pragma once
#include "function/render/render_pass_resource.h"

namespace Zafkiel 
{
class OpenGLRenderPassResourceFactory
{
  public:
    static Scope<RenderPassResource> Create(const Path &path);
};

}