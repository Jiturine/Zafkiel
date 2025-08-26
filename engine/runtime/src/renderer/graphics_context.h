#pragma once
#include "buffers.h"
#include "vertex_array.h"
#include "frame_buffer.h"
#include "shader.h"
#include "base/maths.h"

namespace Zafkiel
{

class GraphicsContext : public RefCounted
{
  public:
    virtual ~GraphicsContext() = default;
    virtual void SetCurrent() = 0;
    virtual Ref<VertexArray> CreateVertexArray() = 0;
    virtual Ref<VertexBuffer> CreateVertexBuffer(uint32_t size) = 0;
    virtual Ref<VertexBuffer> CreateVertexBuffer(float *vertices, uint32_t size) = 0;
    virtual Ref<IndexBuffer> CreateIndexBuffer(uint32_t *indices, uint32_t count) = 0;
    virtual Ref<FrameBuffer> CreateFrameBuffer(const FrameBufferSpecification &spec) = 0;
    virtual void Clear(vec4 color = vec4(0.0f)) = 0;
    virtual void DrawIndexed(Ref<VertexArray> vertexArray, Ref<Shader> shader) = 0;
    virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
    virtual void SwapBuffers() = 0;
    virtual void *GetHandle() const = 0;
};

}