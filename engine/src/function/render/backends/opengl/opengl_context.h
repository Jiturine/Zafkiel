#pragma once
#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include "function/render/graphics_context.h"
#include "function/render/frame_buffer.h"

namespace Zafkiel
{

class OpenGLContext : public GraphicsContext
{
  public:
    OpenGLContext(SDL_Window *window);
    ~OpenGLContext();

    virtual Ref<VertexArray> CreateVertexArray() const override;
    virtual Ref<VertexBuffer> CreateVertexBuffer(uint32_t size) const override;
    virtual Ref<VertexBuffer> CreateVertexBuffer(float *vertices, uint32_t size) const override;
    virtual Ref<IndexBuffer> CreateIndexBuffer(uint32_t *indices, uint32_t count) const override;
    virtual Ref<FrameBuffer> CreateFrameBuffer(const FrameBufferSpecification &spec) const override;
    virtual Ref<Shader> CreateShader(const Path &path) const override;
    virtual Ref<Texture2D> CreateTexture2D(const Path &path) const override;
    virtual Ref<Texture2D> CreateTexture2D(const TextureSpecification &spec, const Buffer &buffer) const override;
    virtual void SetCurrent() override;

    virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
    virtual void Clear(vec4 color) override;
    virtual void DrawIndexed(Ref<VertexArray> vertexArray, Ref<Shader> shader, uint32_t indexCount = 0) override;
    virtual void *GetHandle() const override { return static_cast<void *>(handle); }

  private:
    SDL_Window *window;
    SDL_GLContext handle;
};

}