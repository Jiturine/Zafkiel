#pragma once

namespace Zafkiel
{
// clang-format off
enum class ShaderDataType
{
    None = 0,
    Float, Float2, Float3, Float4,
    Mat3, Mat4,
    Int, Int2, Int3, Int4,
    UInt, UInt2, UInt3, UInt4,
    Bool
};
// clang-format on

uint32_t ShaderDataTypeSize(ShaderDataType type);

class BufferLayout
{
  public:
    struct BufferElement
    {
        BufferElement() = default;
        BufferElement(ShaderDataType type, const std::string &name, bool normalized = false);
        std::string name;
        ShaderDataType type;
        uint32_t size;
        uint32_t offset;
        bool normalized;
        uint32_t GetElementCount() const;
    };
    std::vector<BufferElement> elements;
    uint32_t stride = 0;

    BufferLayout() = default;
    BufferLayout(const std::initializer_list<BufferElement> &elements);
};

class VertexBuffer : public RefCounted
{
  public:
    virtual ~VertexBuffer() {}

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;

    virtual void SetLayout(const BufferLayout &layout) = 0;
    virtual const BufferLayout &GetLayout() const = 0;

    virtual void SetData(const Buffer &buffer) = 0;
};

class IndexBuffer : public RefCounted
{
  public:
    virtual ~IndexBuffer() {}

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;

    virtual uint32_t GetCount() const = 0;
};

class VertexArray : public RefCounted
{
  public:
    virtual ~VertexArray() = default;

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;

    virtual void AddVertexBuffer(const Ref<VertexBuffer> &vertexBuffer) = 0;
    virtual void SetIndexBuffer(const Ref<IndexBuffer> &vertexBuffer) = 0;

    virtual const std::vector<Ref<VertexBuffer>> &GetVertexBuffers() const = 0;
    virtual const Ref<IndexBuffer> &GetIndexBuffer() const = 0;
};
}