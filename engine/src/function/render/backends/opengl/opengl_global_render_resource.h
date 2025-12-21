#pragma once
#include "function/render/global_render_resource.h"

namespace Zafkiel 
{
class OpenGLGlobalRenderResourceFactory
{
  public:
    static Scope<GlobalRenderResource> Create(const Path &path);
};

}