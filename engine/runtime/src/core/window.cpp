#include "window.h"

#include "renderer/backends/opengl/opengl_context.h"

namespace Zafkiel
{
Window::Window(const std::string &title, size_t width, size_t height)
    : title(title), width(width), height(height)
{
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    handle = SDL_CreateWindow(title.c_str(), width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if (!handle)
    {
        Log::CoreError("Could not create a window: {}", SDL_GetError());
    }
    SDL_SetWindowPosition(handle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    SetContext(MakeRef<OpenGLContext>(handle));
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

void Window::SetContext(Ref<GraphicsContext> context)
{
    graphicsContext = context;
    graphicsContext->SetCurrent();
}

}
