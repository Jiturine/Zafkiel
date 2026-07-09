#pragma once
#include "Platform/Input/Keycode.h"
#include "Platform/Cursor.h"
#include "Platform/PlatformWindow/PlatformWindow.h"
#include <SDL3/SDL.h>

namespace Zafkiel
{

class PlatformApplicationEventHandler
{
  public:
    virtual void OnWindowResize(Ref<PlatformWindow> window, uint32 width, uint32 height) = 0;
  
    virtual void OnMouseButtonDown(Ref<PlatformWindow> window, vec2 position, MouseButton button) = 0;

    virtual void OnMouseButtonUp(Ref<PlatformWindow> window, vec2 position, MouseButton button) = 0;

    virtual void OnMouseMove(Ref<PlatformWindow> window, vec2 position) = 0;
};

class PlatformApplication : public Singleton<PlatformApplication, true>
{
  public:
    PlatformApplication();

    ~PlatformApplication();

    void AddPlatformWindow(Ref<PlatformWindow> window);

    void PollEvents();

    void SetEventHandler(PlatformApplicationEventHandler *handler) { eventHandler = handler; }

    void SetCursor(CursorType cursorType);

    bool ShouldQuit() const { return shouldQuit; }

    Ref<PlatformWindow> GetCurrentUnderCursorWindow() { return currentUnderCursorWindow; }

    vec2 GetCurrentCursorPosition() { return currentCursorPosition; }

  private:
    PlatformApplicationEventHandler *eventHandler;

    std::unordered_map<SDL_WindowID, Ref<PlatformWindow>> sdlWindowMap;

    Ref<PlatformWindow> currentUnderCursorWindow;

    vec2 currentCursorPosition = vec2(0.0f);

    std::vector<Ref<PlatformWindow>> platformWindows;

    std::unordered_map<CursorType, SDL_Cursor *> cursors;

    bool shouldQuit = false;
};

}