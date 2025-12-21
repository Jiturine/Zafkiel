#include "input.h"
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_keyboard.h>

namespace Zafkiel
{
bool Input::IsKeyDown(KeyCode key)
{
    if (auto it = keyState.find((uint32_t)key); it != keyState.end())
        return it->second;
    return false;
}

bool Input::IsKeyPressed(KeyCode key)
{
    if (auto it = keyPressed.find((uint32_t)key); it != keyPressed.end())
        return it->second;
    return false;
}
bool Input::IsKeyReleased(KeyCode key)
{
    if (auto it = keyReleased.find((uint32_t)key); it != keyReleased.end())
        return it->second;
    return false;
}

bool Input::IsMouseButtonDown(MouseButton button)
{
    if (auto it = mouseButtonState.find((uint32_t)button); it != mouseButtonState.end())
        return it->second;
    return false;
}

bool Input::IsMouseButtonPressed(MouseButton button)
{
    if (auto it = mouseButtonPressed.find((uint32_t)button); it != mouseButtonPressed.end())
        return it->second;
    return false;
}
bool Input::IsMouseButtonReleased(MouseButton button)
{
    if (auto it = mouseButtonReleased.find((uint32_t)button); it != mouseButtonReleased.end())
        return it->second;
    return false;
}

void Input::ProcessEvent(SDL_Event &e)
{
    switch (e.type)
    {
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        mouseButtonState[e.button.button] = true;
        mouseButtonPressed[e.button.button] = true;
        break;

    case SDL_EVENT_MOUSE_BUTTON_UP:
        mouseButtonState[e.button.button] = false;
        mouseButtonReleased[e.button.button] = true;
        break;

    case SDL_EVENT_KEY_DOWN:
        keyState[e.key.key] = true;
        keyPressed[e.key.key] = true;
        break;

    case SDL_EVENT_KEY_UP:
        keyState[e.key.key] = false;
        keyReleased[e.key.key] = true;
        break;
    }
}

void Input::ClearState()
{
    for (auto &it : keyPressed)
        it.second = false;
    for (auto &it : keyReleased)
        it.second = false;
    for (auto &it : mouseButtonPressed)
        it.second = false;
    for (auto &it : mouseButtonReleased)
        it.second = false;
}
}
