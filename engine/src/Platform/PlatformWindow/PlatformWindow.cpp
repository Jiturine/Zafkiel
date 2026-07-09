#include "Platform/PlatformWindow/PlatformWindow.h"
#include "Platform/Input/Input.h"
#include "Function/RHI/Backends/OpenGL/OpenGLRHI.h"

namespace Zafkiel
{
PlatformWindow::PlatformWindow(const PlatformWindowSpecification &spec)
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

PlatformWindow::~PlatformWindow()
{
    SDL_DestroyWindow(handle);
}
}
