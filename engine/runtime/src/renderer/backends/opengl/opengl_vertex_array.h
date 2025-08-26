#pragma once
#include "../../vertex_array.h"

namespace Zafkiel
{
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