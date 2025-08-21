#pragma once
#include <SDL3/SDL.h>
#include "renderer/graphics_context.h"
#include "renderer/opengl_context.h"
#include "event.h"

namespace Zafkiel
{
class Window
{
  public:
    Window(const std::string &title, size_t width, size_t height);
    virtual ~Window();
    void OnUpdate(float timestep);
    bool ShouldClose() { return shouldClose; }
    virtual void OnEvent(Event &event);
    SDL_Window *GetHandle() { return handle; }

  protected:
    SDL_Window *handle;
    std::unique_ptr<GraphicsContext> graphicsContext;
    size_t width;
    size_t height;
    std::string title;
    bool shouldClose = false;
};
}
