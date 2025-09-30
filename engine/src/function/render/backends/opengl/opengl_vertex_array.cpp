#include "opengl_vertex_array.h"
#include <glad/glad.h>

namespace Zafkiel
{
static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
{
    switch (type)
    {
        using enum ShaderDataType;
    case Float: return GL_FLOAT;
    case Float2: return GL_FLOAT;
    case Float3: return GL_FLOAT;
    case Float4: return GL_FLOAT;
    case Mat3: return GL_FLOAT;
    case Mat4: return GL_FLOAT;
    case Int: return GL_INT;
    case Int2: return GL_INT;
    case Int3: return GL_INT;
    case Int4: return GL_INT;
    case UInt: return GL_UNSIGNED_INT;
    case UInt2: return GL_UNSIGNED_INT;
    case UInt3: return GL_UNSIGNED_INT;
    case UInt4: return GL_UNSIGNED_INT;
    case Bool: return GL_BOOL;
    case None:
        Log::CoreError("Unknown ShaderDataType!");
        break;
    }
    return 0;
}

OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t size)
{
    glCreateBuffers(1, &rendererID);
    glBindBuffer(GL_ARRAY_BUFFER, rendererID);
    glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
}
OpenGLVertexBuffer::OpenGLVertexBuffer(const float *vertices, uint32_t size)
{
    glCreateBuffers(1, &rendererID);
    glBindBuffer(GL_ARRAY_BUFFER, rendererID);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}
OpenGLVertexBuffer::~OpenGLVertexBuffer()
{
    glDeleteBuffers(1, &rendererID);
}
void OpenGLVertexBuffer::Bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, rendererID);
}
void OpenGLVertexBuffer::Unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void OpenGLVertexBuffer::SetData(const Buffer &buffer)
{
    glBindBuffer(GL_ARRAY_BUFFER, rendererID);
    glBufferSubData(GL_ARRAY_BUFFER, 0, buffer.size(), buffer.data());
}

OpenGLIndexBuffer::OpenGLIndexBuffer(const uint32_t *vertices, uint32_t count)
    : count(count)
{
    glCreateBuffers(1, &rendererID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), vertices, GL_STATIC_DRAW);
}
OpenGLIndexBuffer::~OpenGLIndexBuffer()
{
    glDeleteBuffers(1, &rendererID);
}
void OpenGLIndexBuffer::Bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererID);
}
void OpenGLIndexBuffer::Unbind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

OpenGLVertexArray::OpenGLVertexArray()
{
    glCreateVertexArrays(1, &rendererID);
}
OpenGLVertexArray::~OpenGLVertexArray()
{
    glDeleteVertexArrays(1, &rendererID);
}
void OpenGLVertexArray::Bind() const
{
    glBindVertexArray(rendererID);
}
void OpenGLVertexArray::Unbind() const
{
    glBindVertexArray(0);
}
void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer> &vertexBuffer)
{
    if (vertexBuffer->GetLayout().elements.empty())
    {
        Log::CoreError("Vertex buffer has no layout!");
    }
    glBindVertexArray(rendererID);
    vertexBuffer->Bind();
    uint32_t index = 0;
    const auto &layout = vertexBuffer->GetLayout();
    for (const auto &element : layout.elements)
    {
        switch (element.type)
        {
            using enum ShaderDataType;
        case Float:
        case Float2:
        case Float3:
        case Float4:
        case Mat3:
        case Mat4:
            glEnableVertexAttribArray(index);
            glVertexAttribPointer(
                index,
                element.GetElementCount(),
                ShaderDataTypeToOpenGLBaseType(element.type),
                element.normalized ? GL_TRUE : GL_FALSE,
                layout.stride,
                reinterpret_cast<const void *>(element.offset));
            index++;
            break;
        case Int:
        case Int2:
        case Int3:
        case Int4:
        case UInt:
        case UInt2:
        case UInt3:
        case UInt4:
        case Bool:
            glEnableVertexAttribArray(index);
            glVertexAttribIPointer(
                index,
                element.GetElementCount(),
                ShaderDataTypeToOpenGLBaseType(element.type),
                layout.stride,
                reinterpret_cast<const void *>(element.offset));
            index++;
            break;
        default: break;
        }
        vertexBuffers.push_back(vertexBuffer);
    }
}
void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer> &indexBuffer)
{
    glBindVertexArray(rendererID);
    indexBuffer->Bind();
    this->indexBuffer = indexBuffer;
}
}