#include "opengl_context.h"
#include <SDL3/SDL_opengl.h>

namespace Zafkiel
{

OpenGLContext::OpenGLContext(SDL_Window *window)
    : window(window)
{
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    handle = SDL_GL_CreateContext(window);
    if (!handle)
    {
        Log::CoreError("Could not create an OpenGL context: {}", SDL_GetError());
    }

    SDL_GL_MakeCurrent(window, handle);
}

void OpenGLContext::SwapBuffers()
{
    SDL_GL_SwapWindow(window);
}

void OpenGLContext::Clear()
{
    glClear(GL_COLOR_BUFFER_BIT);
}

}