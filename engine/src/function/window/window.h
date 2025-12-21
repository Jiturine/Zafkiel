#pragma once
#include <SDL3/SDL.h>
#include "function/render/graphics_context.h"
#include "function/input/event.h"

namespace Zafkiel
{

struct WindowSpecification
{
    GraphicsAPI graphicsAPI;
    std::string title;
    uint32_t width;
    uint32_t height;
};

class Window
{
  public:
    Window(const WindowSpecification &spec);
    virtual ~Window();
    void PollEvents();
    void SwapBuffers();
    bool ShouldClose() { return shouldClose; }
    virtual void OnEvent(SDL_Event &event);
    SDL_Window *GetHandle() const { return handle; }
    uint32_t GetWidth() const { return width; }
    uint32_t GetHeight() const { return height; }

  protected:
    SDL_Window *handle;
    size_t width;
    size_t height;
    std::string title;
    bool shouldClose = false;
};
}
