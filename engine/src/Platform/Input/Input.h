#pragma once
#include "Platform/Input/Keycode.h"
#include <SDL3/SDL.h>

namespace Zafkiel
{
class Input
{
  public:
    static bool IsKeyDown(KeyCode key);
    static bool IsKeyReleased(KeyCode key);
    static bool IsKeyPressed(KeyCode key);
    static bool IsMouseButtonDown(MouseButton button);
    static bool IsMouseButtonReleased(MouseButton button);
    static bool IsMouseButtonPressed(MouseButton button);
    static void ProcessEvent(SDL_Event &e); // 未实际调用！
    static void ClearState();

  private:
    static inline std::unordered_map<uint32, bool> mouseButtonState;
    static inline std::unordered_map<uint32, bool> mouseButtonPressed;
    static inline std::unordered_map<uint32, bool> mouseButtonReleased;
    static inline std::unordered_map<SDL_Keycode, bool> keyState;
    static inline std::unordered_map<SDL_Keycode, bool> keyPressed;
    static inline std::unordered_map<SDL_Keycode, bool> keyReleased;
};
}