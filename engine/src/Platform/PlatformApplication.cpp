#include "Platform/PlatformApplication.h"
#include <SDL3/SDL_events.h>

namespace Zafkiel
{

PlatformApplication::PlatformApplication()
{
    cursors[CursorType::ResizeUpDown] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NS_RESIZE);
    cursors[CursorType::ResizeLeftRight] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_EW_RESIZE);
    cursors[CursorType::ResizeSouthEast] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NWSE_RESIZE);
    cursors[CursorType::ResizeSouthWest] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NESW_RESIZE);
}

PlatformApplication::~PlatformApplication()
{
    for (auto [cursurType, cursor] : cursors)
    {
        SDL_DestroyCursor(cursor);
    }
}

void PlatformApplication::AddPlatformWindow(Ref<PlatformWindow> window)
{
    platformWindows.push_back(window);
    sdlWindowMap[window->GetHandleID()] = window;
}

void PlatformApplication::PollEvents()
{
    SDL_Event sdl_event;
    while (SDL_PollEvent(&sdl_event))
    {
        switch (sdl_event.type)
        {
        case SDL_EVENT_QUIT:
            shouldQuit = true;
            break;
        case SDL_EVENT_WINDOW_MOUSE_ENTER:
        {
            uint32 windowHandle = sdl_event.window.windowID;
            auto window = sdlWindowMap.at(windowHandle);

            currentUnderCursorWindow = window;
            break;
        }
        case SDL_EVENT_WINDOW_MOUSE_LEAVE:
        {
            currentUnderCursorWindow = nullptr;
            break;
        }
        case SDL_EVENT_MOUSE_MOTION:
        {
            uint32 windowHandle = sdl_event.motion.windowID;
            currentUnderCursorWindow = sdlWindowMap.at(windowHandle);
            
            float x = sdl_event.motion.x;
            float y = sdl_event.motion.y;

            currentCursorPosition = vec2(x, y);

            eventHandler->OnMouseMove(currentUnderCursorWindow, vec2(x, y));
            break;
        }
        case SDL_EVENT_WINDOW_RESIZED:
        {
            uint32 width = sdl_event.window.data1;
            uint32 height = sdl_event.window.data2;
            uint32 windowHandle = sdl_event.window.windowID;
            auto window = sdlWindowMap.at(windowHandle);

            eventHandler->OnWindowResize(window, width, height);
            break;
        }
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        {
            float x = sdl_event.button.x;
            float y = sdl_event.button.y;
            MouseButton mouseButton = sdl_event.button.button == SDL_BUTTON_LEFT ? MouseButton::Left :
                                      sdl_event.button.button == SDL_BUTTON_RIGHT ? MouseButton::Right :
                                      sdl_event.button.button == SDL_BUTTON_MIDDLE ? MouseButton::Middle : MouseButton::None;

            uint32 windowHandle = sdl_event.window.windowID;
            auto window = sdlWindowMap.at(windowHandle);

            eventHandler->OnMouseButtonDown(window, vec2(x, y), mouseButton);
            break;
        }
        case SDL_EVENT_MOUSE_BUTTON_UP:
        {
            float x = sdl_event.button.x;
            float y = sdl_event.button.y;
            MouseButton mouseButton = sdl_event.button.button == SDL_BUTTON_LEFT ? MouseButton::Left :
                                      sdl_event.button.button == SDL_BUTTON_RIGHT ? MouseButton::Right :
                                      sdl_event.button.button == SDL_BUTTON_MIDDLE ? MouseButton::Middle : MouseButton::None;
            uint32 windowHandle = sdl_event.window.windowID;
            auto window = sdlWindowMap.at(windowHandle);

            eventHandler->OnMouseButtonUp(window, vec2(x, y), mouseButton);
            break;
        }
        }
    }
}

void PlatformApplication::SetCursor(CursorType cursorType)
{
    if (cursorType == CursorType::Default)
        SDL_SetCursor(SDL_GetDefaultCursor());
    else
        SDL_SetCursor(cursors[cursorType]);
}
  
}