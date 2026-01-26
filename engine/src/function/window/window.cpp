#include "function/window/window.h"
#include "function/input/input.h"
#include "function/render/backends/opengl/opengl_context.h"

namespace Zafkiel
{
Window::Window(const WindowSpecification &spec)
    : title(spec.title), width(spec.width), height(spec.height)
{
    if (spec.graphicsAPI == GraphicsAPI::OpenGL)
    {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    
        handle = SDL_CreateWindow(title.c_str(), width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    }
    else if (spec.graphicsAPI == GraphicsAPI::Vulkan)
    {
        handle = SDL_CreateWindow(title.c_str(), width, height, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
        
    }
    if (!handle)
    {
        Log::Error("Could not create a window: {}", SDL_GetError());
    }
    SDL_SetWindowPosition(handle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
}

Window::~Window()
{
    SDL_DestroyWindow(handle);
}

void Window::PollEvents()
{
    SDL_Event sdl_event;
    while (SDL_PollEvent(&sdl_event))
    {
        OnEvent(sdl_event);

        switch (sdl_event.type)
        {
        case SDL_EVENT_QUIT:
            shouldClose = true;
            break;
        }
    }
}

void Window::SwapBuffers()
{
    SDL_GL_SwapWindow(handle);
}

void Window::OnEvent(SDL_Event &event)
{
    Input::ProcessEvent(event);
}

}
