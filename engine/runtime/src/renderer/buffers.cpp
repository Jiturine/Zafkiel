#include "buffers.h"

namespace Zafkiel
{

uint32_t ShaderDataTypeSize(ShaderDataType type)
{
    switch (type)
    {
        using enum ShaderDataType;
    case Float: return 4;
    case Float2: return 4 * 2;
    case Float3: return 4 * 3;
    case Float4: return 4 * 4;
    case Mat3: return 4 * 3 * 3;
    case Mat4: return 4 * 4 * 4;
    case Int: return 4;
    case Int2: return 4 * 2;
    case Int3: return 4 * 3;
    case Int4: return 4 * 4;
    case Bool: return 1;
    case None:
        Log::CoreError("Unknown ShaderDataType!");
        break;
    }
    return 0;
}

BufferLayout::BufferElement::BufferElement(ShaderDataType type, const std::string &name, bool normalized)
    : name(name), type(type), size(ShaderDataTypeSize(type)), offset(0), normalized(normalized) {}

uint32_t BufferLayout::BufferElement::GetElementCount() const
{
    switch (type)
    {
        using enum ShaderDataType;
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
    case Bool: return 1;
    case None:
        Log::CoreError("Unknown ShaderDataType!");
        break;
    }
    return 0;
}

BufferLayout::BufferLayout(const std::initializer_list<BufferElement> &elements)
    : elements(elements)
{
    uint32_t offset = 0;
    stride = 0;
    for (auto &element : this->elements)
    {
        element.offset = offset;
        offset += element.size;
        stride += element.size;
    }
}

}
