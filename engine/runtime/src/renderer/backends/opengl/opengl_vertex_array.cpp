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
    case Bool: return GL_BOOL;
    case None:
        Log::CoreError("Unknown ShaderDataType!");
        break;
    }
    return 0;
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