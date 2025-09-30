#pragma once
#include "keycode.h"
#include <SDL3/SDL.h>

namespace Zafkiel
{
class Input
{
  public:
    static bool IsKeyPressed(Scancode key);
    static bool IsMouseButtonDown(MouseButton button);
    static bool IsMouseButtonUp(MouseButton button);
    static bool IsMouseButtonPressed(MouseButton button);
    static void ProcessEvent(SDL_Event &e);
    static void ClearState();
  private:
    static inline bool mouseDown[6] = {false};
    static inline bool mouseUp[6] = {false};
    static inline bool mouseCurrent[6] = {false};
};
}