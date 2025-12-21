#include "opengl_index_buffer.h"
#include <glad/glad.h>

namespace Zafkiel
{

OpenGLIndexBufferBackend::OpenGLIndexBufferBackend(const uint32_t *vertices, uint32_t count)
    : count(count)
{
    glCreateBuffers(1, &rendererID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), vertices, GL_STATIC_DRAW);
}
OpenGLIndexBufferBackend::~OpenGLIndexBufferBackend()
{
    glDeleteBuffers(1, &rendererID);
}

Scope<IndexBuffer> OpenGLIndexBufferFactory::Create(const uint32_t* indices, uint32_t count)
{
    auto backend = CreateScope<OpenGLIndexBufferBackend>(indices, count);
    return CreateScope<IndexBuffer>(indices, count, std::move(backend));
}
// void OpenGLIndexBufferBackend::Bind() const
// {
//     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererID);
// }
// void OpenGLIndexBufferBackend::Unbind() const
// {
//     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
// }

// OpenGLVertexArray::OpenGLVertexArray()
// {
//     glCreateVertexArrays(1, &rendererID);
// }
// OpenGLVertexArray::~OpenGLVertexArray()
// {
//     glDeleteVertexArrays(1, &rendererID);
// }
// void OpenGLVertexArray::Bind() const
// {
//     glBindVertexArray(rendererID);
// }
// void OpenGLVertexArray::Unbind() const
// {
//     glBindVertexArray(0);
// }
// void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer> &vertexBuffer)
// {
//     if (vertexBuffer->GetLayout().elements.empty())
//     {
//         Log::Error("Vertex buffer has no layout!");
//     }
//     glBindVertexArray(rendererID);
//     vertexBuffer->Bind();
//     uint32_t index = 0;
//     const auto &layout = vertexBuffer->GetLayout();
//     for (const auto &element : layout.elements)
//     {
//         switch (element.type)
//         {
//             using enum ShaderDataType;
//         case Float:
//         case Float2:
//         case Float3:
//         case Float4:
//         case Mat3:
//         case Mat4:
//             glEnableVertexAttribArray(index);
//             glVertexAttribPointer(
//                 index,
//                 element.GetElementCount(),
//                 ShaderDataTypeToOpenGLBaseType(element.type),
//                 element.normalized ? GL_TRUE : GL_FALSE,
//                 layout.stride,
//                 reinterpret_cast<const void *>(element.offset));
//             index++;
//             break;
//         case Int:
//         case Int2:
//         case Int3:
//         case Int4:
//         case UInt:
//         case UInt2:
//         case UInt3:
//         case UInt4:
//         case Bool:
//             glEnableVertexAttribArray(index);
//             glVertexAttribIPointer(
//                 index,
//                 element.GetElementCount(),
//                 ShaderDataTypeToOpenGLBaseType(element.type),
//                 layout.stride,
//                 reinterpret_cast<const void *>(element.offset));
//             index++;
//             break;
//         default: break;
//         }
//         vertexBuffers.push_back(vertexBuffer);
//     }
// }
// void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer> &indexBuffer)
// {
//     glBindVertexArray(rendererID);
//     indexBuffer->Bind();
//     this->indexBuffer = indexBuffer;
// }
}
