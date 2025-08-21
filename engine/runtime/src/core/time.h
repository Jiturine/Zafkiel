#pragma once
#include <SDL3/SDL.h>

namespace Zafkiel
{
class Time
{
  public:
    static float Now() { return SDL_GetTicks() / 1000.0f; }
};
}