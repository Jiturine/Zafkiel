#include "opengl_vertex_buffer.h"
#include <glad/glad.h>

namespace Zafkiel
{
GLenum ShaderDataTypeToOpenGLBaseType(ShaderFundamentalType type)
{
    switch (type)
    {
        using enum ShaderFundamentalType;
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
        Log::Error("Unknown ShaderDataType!");
        break;
    }
    return 0;
}

uint32_t ShaderDataTypeCount(ShaderFundamentalType type)
{
    switch (type)
    {
        using enum ShaderFundamentalType;
    case Float: return 1;
    case Float2: return 2;
    case Float3: return 3;
    case Float4: return 4;
    case Mat3: return 3 * 3;
    case Mat4: return 4 * 4;
    case Int: return 1;
    case Int2: return 2;
    case Int3: return 3;
    case Int4: return 4;
    case UInt: return 1;
    case UInt2: return 2;
    case UInt3: return 3;
    case UInt4: return 4;
    case Bool: return 1;
    case None:
        Log::Error("Unknown ShaderDataType!");
        break;
    }
    return 0;
}

OpenGLVertexBufferBackend::OpenGLVertexBufferBackend(const float *vertices, uint32_t size)
{
    glCreateBuffers(1, &rendererID);
    glBindBuffer(GL_ARRAY_BUFFER, rendererID);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
}
OpenGLVertexBufferBackend::~OpenGLVertexBufferBackend()
{
    glDeleteBuffers(1, &rendererID);
}
/* void OpenGLVertexBufferBackend::Bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, rendererID);
}
void OpenGLVertexBufferBackend::Unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void OpenGLVertexBufferBackend::SetData(Buffer buffer)
{
    glBindBuffer(GL_ARRAY_BUFFER, rendererID);
    glBufferSubData(GL_ARRAY_BUFFER, 0, buffer.size(), buffer.data());
} */

Scope<VertexBuffer> OpenGLVertexBufferFactory::Create(const float *vertices, uint32_t size)
{
    auto backend = CreateScope<OpenGLVertexBufferBackend>(vertices, size);
    return CreateScope<VertexBuffer>(vertices, size, std::move(backend));
}

}
