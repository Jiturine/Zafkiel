#pragma once
#include "Core/Base/GraphicsAPI.h"
#include "Platform/Input/Keycode.h"
#include <SDL3/SDL.h>

namespace Zafkiel
{

struct PlatformWindowSpecification
{
    GraphicsAPI graphicsAPI;
    std::string title;
    uint32 width;
    uint32 height;
};

class PlatformWindow : public RefCounted
{
  public:
    PlatformWindow(const PlatformWindowSpecification &spec);

    virtual ~PlatformWindow();
    
    SDL_Window *GetHandle() { return handle; }

    SDL_WindowID GetHandleID() { return SDL_GetWindowID(handle); }
  
    uint32 GetWidth() const { return width; }
    uint32 GetHeight() const { return height; }

    void SetWidth(uint32 newWidth) { width = newWidth; }
    void SetHeight(uint32 newHeight) { height = newHeight; }

  protected:
    SDL_Window *handle;
  
    size_t width;
    size_t height;

    std::string title;
};
}
