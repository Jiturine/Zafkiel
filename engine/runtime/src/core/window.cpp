#include "window.h"

namespace Zafkiel
{
Window::Window(const std::string &title, size_t width, size_t height)
    : title(title), width(width), height(height)
{
    handle = SDL_CreateWindow(title.c_str(), width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!handle)
    {
        Log::CoreError("Could not create a window: {}", SDL_GetError());
    }
    SDL_SetWindowPosition(handle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    graphicsContext = std::make_unique<OpenGLContext>(handle);
}

Window::~Window()
{
    SDL_DestroyWindow(handle);
}

void Window::OnUpdate(float timestep)
{
    SDL_Delay(16);

    SDL_Event sdl_event;
    while (SDL_PollEvent(&sdl_event))
    {
        Event event(sdl_event);
        OnEvent(event);

        switch (sdl_event.type)
        {
        case SDL_EVENT_QUIT:
            shouldClose = true;
            break;
        }
    }
    graphicsContext->SwapBuffers();
    graphicsContext->Clear();
}

void Window::OnEvent(Event &event)
{
}

}
