#pragma once
#include <SDL3/SDL.h>

namespace Zafkiel
{

class Event
{
  public:
    Event(SDL_Event &e) : handle(e) {}

    SDL_Event &GetHandle() { return handle; }

  private:
    SDL_Event &handle;
};
}