#include "opengl_context.h"
#include <SDL3/SDL_opengl.h>
#include "opengl_vertex_array.h"
#include "opengl_frame_buffer.h"
#include "opengl_shader.h"
#include "opengl_texture.h"

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

Ref<VertexArray> OpenGLContext::CreateVertexArray() const
{
    return MakeRef<OpenGLVertexArray>();
}
Ref<VertexBuffer> OpenGLContext::CreateVertexBuffer(uint32_t size) const
{
    return MakeRef<OpenGLVertexBuffer>(size);
}
Ref<VertexBuffer> OpenGLContext::CreateVertexBuffer(float *vertices, uint32_t size) const
{
    return MakeRef<OpenGLVertexBuffer>(vertices, size);
}
Ref<IndexBuffer> OpenGLContext::CreateIndexBuffer(uint32_t *indices, uint32_t count) const
{
    return MakeRef<OpenGLIndexBuffer>(indices, count);
}
Ref<FrameBuffer> OpenGLContext::CreateFrameBuffer(const FrameBufferSpecification &spec) const
{
    return MakeRef<OpenGLFrameBuffer>(spec);
}
Ref<Shader> OpenGLContext::CreateShader(const Path &path) const
{
    return MakeRef<OpenGLShader>(path);
}
Ref<Texture2D> OpenGLContext::CreateTexture2D(const Path &path) const
{
    return MakeRef<OpenGLTexture2D>(path);
}
Ref<Texture2D> OpenGLContext::CreateTexture2D(const TextureSpecification &spec, const Buffer &buffer) const
{
    return MakeRef<OpenGLTexture2D>(spec, buffer);
}

void OpenGLContext::Clear(vec4 color)
{
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLContext::DrawIndexed(Ref<VertexArray> vertexArray, Ref<Shader> shader, uint32_t indexCount)
{
    vertexArray->Bind();
    shader->Bind();
    auto count = indexCount == 0 ? vertexArray->GetIndexBuffer()->GetCount() : indexCount;
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
}

void OpenGLContext::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    glViewport(x, y, width, height);
}

}