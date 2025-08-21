#pragma once
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include "graphics_context.h"

namespace Zafkiel
{

class OpenGLContext : public GraphicsContext
{
  public:
    OpenGLContext(SDL_Window *window);

    void SwapBuffers() override;
    void Clear() override;
    void *GetHandle() const override { return static_cast<void *>(handle); }

  private:
    SDL_Window *window;
    SDL_GLContext handle;
};

}