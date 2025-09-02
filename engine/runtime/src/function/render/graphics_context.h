#pragma once
#include "vertex_array.h"
#include "frame_buffer.h"
#include "shader.h"
#include "texture.h"
#include "platform/filesystem/filesystem.h"

namespace Zafkiel
{

class GraphicsContext : public RefCounted
{
  public:
    virtual ~GraphicsContext() = default;
    virtual void SetCurrent() = 0;
    virtual Ref<VertexArray> CreateVertexArray() const = 0;
    virtual Ref<VertexBuffer> CreateVertexBuffer(uint32_t size) const = 0;
    virtual Ref<VertexBuffer> CreateVertexBuffer(float *vertices, uint32_t size) const = 0;
    virtual Ref<IndexBuffer> CreateIndexBuffer(uint32_t *indices, uint32_t count) const = 0;
    virtual Ref<FrameBuffer> CreateFrameBuffer(const FrameBufferSpecification &spec) const = 0;
    virtual Ref<Shader> CreateShader(const Path &path) const = 0;
    virtual Ref<Texture2D> CreateTexture2D(const Path &path) const = 0;
    virtual Ref<Texture2D> CreateTexture2D(const TextureSpecification &spec, const Buffer &buffer) const = 0;
    virtual void Clear(vec4 color = vec4(0.0f)) = 0;
    virtual void DrawIndexed(Ref<VertexArray> vertexArray, Ref<Shader> shader, uint32_t indexCount = 0) = 0;
    virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
    virtual void SwapBuffers() = 0;
    virtual void *GetHandle() const = 0;
};

}