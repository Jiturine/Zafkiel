#include "Function/RHI/Backends/OpenGL/OpenGLBuffer.h"

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

uint32 ShaderDataTypeCount(ShaderFundamentalType type)
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

OpenGLBuffer::OpenGLBuffer(const RHIBufferDesc &desc, const void *data)
    : RHIBuffer(desc)
{
    glCreateBuffers(1, &handle);
    
    GLenum target;
    if (EnumHasAnyFlags(desc.usages, BufferUsageFlags::VertexBuffer))
    {
        target = GL_ARRAY_BUFFER;
    }
    else if (EnumHasAnyFlags(desc.usages, BufferUsageFlags::IndexBuffer))
    {
        target = GL_ELEMENT_ARRAY_BUFFER;
    }
    else if (EnumHasAnyFlags(desc.usages, BufferUsageFlags::UniformBuffer))
    {
        target = GL_UNIFORM_BUFFER;
    }
    else 
    {
        Log::Error("Unknown BufferTarget!");
    }

    glBindBuffer(target, handle);

    GLenum usage;
    if (EnumHasAnyFlags(desc.usages, BufferUsageFlags::Static))
    {
        usage = GL_STATIC_DRAW;
    }
    else if (EnumHasAnyFlags(desc.usages, BufferUsageFlags::Dynamic))
    {
        usage = GL_DYNAMIC_DRAW;
    }
    else 
    {
        Log::Error("Unknown BufferUsage!");
    }

    glBufferData(target, desc.size, data, usage);
}

OpenGLBuffer::~OpenGLBuffer()
{
    glDeleteBuffers(1, &handle);
}

GLenum OpenGLBuffer::GetType() const
{
    if (EnumHasAnyFlags(desc.usages, BufferUsageFlags::VertexBuffer))
        return GL_ARRAY_BUFFER;
    else if (EnumHasAnyFlags(desc.usages, BufferUsageFlags::IndexBuffer))
        return GL_ELEMENT_ARRAY_BUFFER;
    else if (EnumHasAnyFlags(desc.usages, BufferUsageFlags::UniformBuffer))
        return GL_UNIFORM_BUFFER;
    else Log::Error("Unknown GLBuffer Type!");
    return GL_NONE;
}

}
