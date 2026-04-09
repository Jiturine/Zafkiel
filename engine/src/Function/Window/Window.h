#pragma once
#include "Function/RHI/GraphicsAPI.h"
#include <SDL3/SDL.h>

namespace Zafkiel
{

struct WindowSpecification
{
    GraphicsAPI graphicsAPI;
    std::string title;
    uint32 width;
    uint32 height;
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
    
    SDL_Window *GetHandle() { return handle; }
  
    uint32 GetWidth() const { return width; }
    uint32 GetHeight() const { return height; }

  protected:
    SDL_Window *handle;
  
    size_t width;
    size_t height;

    std::string title;
    
    bool shouldClose = false;
};
}
