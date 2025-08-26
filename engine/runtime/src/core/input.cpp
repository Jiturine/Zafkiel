#include "input.h"
#include <SDL3/SDL_keyboard.h>

namespace Zafkiel
{
bool Input::IsKeyPressed(Scancode key)
{
    return SDL_GetKeyboardState(nullptr)[(int)key];
}
}