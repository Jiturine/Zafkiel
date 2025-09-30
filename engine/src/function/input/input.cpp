#include "input.h"
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_keyboard.h>

namespace Zafkiel
{
bool Input::IsKeyPressed(Scancode key)
{
    return SDL_GetKeyboardState(nullptr)[(int)key];
}
bool Input::IsMouseButtonPressed(MouseButton button)
{
    float mouseX, mouseY;
    SDL_MouseButtonFlags mouseButtons = SDL_GetMouseState(&mouseX, &mouseY);
    if (button == MouseButton::Left && (mouseButtons & SDL_BUTTON_MASK(SDL_BUTTON_LEFT)))
        return true;

    if (button == MouseButton::Right && (mouseButtons & SDL_BUTTON_MASK(SDL_BUTTON_RIGHT)))
        return true;

    return false;
}

bool Input::IsMouseButtonDown(MouseButton button)
{
    if (button == MouseButton::Left)
        return mouseDown[SDL_BUTTON_LEFT];
    if (button == MouseButton::Right)
        return mouseDown[SDL_BUTTON_RIGHT];
    return false;
}

bool Input::IsMouseButtonUp(MouseButton button)
{
    if (button == MouseButton::Left)
        return mouseUp[SDL_BUTTON_LEFT];
    if (button == MouseButton::Right)
        return mouseUp[SDL_BUTTON_RIGHT];
    return false;
}

void Input::ProcessEvent(SDL_Event &e)
{
    switch (e.type)
    {
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
        if (e.button.button >= 1 && e.button.button <= 5)
        {
            mouseCurrent[e.button.button] = true;
            mouseDown[e.button.button] = true;
        }
        break;

    case SDL_EVENT_MOUSE_BUTTON_UP:
        if (e.button.button >= 1 && e.button.button <= 5)
        {
            mouseCurrent[e.button.button] = false;
            mouseUp[e.button.button] = true;
        }
        break;
    }
}

void Input::ClearState()
{
    for (int i = 0; i < 6; i++)
    {
        mouseCurrent[i] = false;
        mouseDown[i] = false;
        mouseUp[i] = false;
    }
}
}