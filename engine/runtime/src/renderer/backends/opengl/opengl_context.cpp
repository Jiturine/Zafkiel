#include "opengl_context.h"
#include <SDL3/SDL_opengl.h>
#include "opengl_vertex_array.h"
#include "opengl_buffers.h"
#include "opengl_frame_buffer.h"

namespace Zafkiel
{

OpenGLContext::OpenGLContext(SDL_Window *window)
    : window(window)
{
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    handle = SDL_GL_CreateContext(window);
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        Log::CoreCritical("Failed to initialize glad!");
    }

    if (!handle)
    {
        Log::CoreError("Could not create an OpenGL context: {}", SDL_GetError());
    }
    else
    {
        Log::CoreInfo("OpenGL Info:");
        Log::CoreInfo("  Vendor: {}", (const char *)glGetString(GL_VENDOR));
        Log::CoreInfo("  Renderer: {}", (const char *)glGetString(GL_RENDERER));
        Log::CoreInfo("  Version: {}", (const char *)glGetString(GL_VERSION));
    }
}

OpenGLContext::~OpenGLContext()
{
    SDL_GL_DestroyContext(handle);
}

void OpenGLContext::SetCurrent()
{
    SDL_GL_MakeCurrent(window, handle);
}

Ref<VertexArray> OpenGLContext::CreateVertexArray()
{
    return MakeRef<OpenGLVertexArray>();
}
Ref<VertexBuffer> OpenGLContext::CreateVertexBuffer(uint32_t size)
{
    return MakeRef<OpenGLVertexBuffer>(size);
}
Ref<VertexBuffer> OpenGLContext::CreateVertexBuffer(float *vertices, uint32_t size)
{
    return MakeRef<OpenGLVertexBuffer>(vertices, size);
}
Ref<IndexBuffer> OpenGLContext::CreateIndexBuffer(uint32_t *indices, uint32_t count)
{
    return MakeRef<OpenGLIndexBuffer>(indices, count);
}
Ref<FrameBuffer> OpenGLContext::CreateFrameBuffer(const FrameBufferSpecification &spec)
{
    return MakeRef<OpenGLFrameBuffer>(spec);
}

void OpenGLContext::SwapBuffers()
{
    SDL_GL_SwapWindow(window);
}

void OpenGLContext::Clear(vec4 color)
{
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLContext::DrawIndexed(Ref<VertexArray> vertexArray, Ref<Shader> shader)
{
    vertexArray->Bind();
    shader->Bind();
    glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
}

void OpenGLContext::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    glViewport(x, y, width, height);
}

}