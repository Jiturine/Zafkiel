#pragma once
#include "function/render/vertex_array.h"

namespace Zafkiel
{
class OpenGLVertexBuffer : public VertexBuffer
{
  public:
    OpenGLVertexBuffer(uint32_t size);
    OpenGLVertexBuffer(float *vertices, uint32_t size);
    virtual ~OpenGLVertexBuffer();

    virtual void Bind() const override;
    virtual void Unbind() const override;
    virtual void SetData(const Buffer &buffer) override;

    virtual const BufferLayout &GetLayout() const override { return layout; }
    virtual void SetLayout(const BufferLayout &layout) override { this->layout = layout; }

  private:
    uint32_t rendererID;
    BufferLayout layout;
};
class OpenGLIndexBuffer : public IndexBuffer
{
  public:
    OpenGLIndexBuffer(uint32_t *indices, uint32_t count);
    virtual ~OpenGLIndexBuffer();

    virtual void Bind() const override;
    virtual void Unbind() const override;

    virtual uint32_t GetCount() const override
    {
        return count;
    }

  private:
    uint32_t rendererID;
    uint32_t count;
};

class OpenGLVertexArray : public VertexArray
{
  public:
    OpenGLVertexArray();
    virtual ~OpenGLVertexArray();
    virtual void Bind() const override;
    virtual void Unbind() const override;

    virtual void AddVertexBuffer(const Ref<VertexBuffer> &vertexBuffer) override;
    virtual void SetIndexBuffer(const Ref<IndexBuffer> &vertexBuffer) override;

    virtual const std::vector<Ref<VertexBuffer>> &GetVertexBuffers() const override
    {
        return vertexBuffers;
    }
    virtual const Ref<IndexBuffer> &GetIndexBuffer() const override
    {
        return indexBuffer;
    }

  private:
    uint32_t rendererID;
    std::vector<Ref<VertexBuffer>> vertexBuffers;
    Ref<IndexBuffer> indexBuffer;
};
}