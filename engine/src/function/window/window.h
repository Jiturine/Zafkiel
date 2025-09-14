#pragma once
#include <SDL3/SDL.h>
#include "function/render/graphics_context.h"
#include "function/input/event.h"

namespace Zafkiel
{
class Window : public RefCounted
{
  public:
    Window(const std::string &title, size_t width, size_t height);
    virtual ~Window();
    void SetContext(Ref<GraphicsContext> context);
    void OnUpdate(float timestep);
    bool ShouldClose() { return shouldClose; }
    virtual void OnEvent(Event &event);
    SDL_Window *GetHandle() { return handle; }

  protected:
    SDL_Window *handle;
    Ref<GraphicsContext> graphicsContext;
    size_t width;
    size_t height;
    std::string title;
    bool shouldClose = false;
};
}
