#pragma once
#include "function/render/shader.h"
#include "function/render/render_handle.h"
#include "function/render/shader_reflection.h"

namespace Zafkiel
{

struct VertexBufferElementLayout
{
    std::string name;
    uint32_t location;
    uint32_t size;
    uint32_t offset;
    ShaderFundamentalType type;
};

struct VertexBufferLayout
{
    std::vector<VertexBufferElementLayout> elements;
    uint32_t stride;
};

class VertexBufferBackend 
{
  public:
    virtual ~VertexBufferBackend() = default;
};

class VertexBuffer final
{
  public:
    VertexBuffer(uint32_t size, Scope<VertexBufferBackend> backend)
        : size(size), backend(std::move(backend)) {}

    Borrow<VertexBufferBackend> GetBackend() const { return Borrow(backend); }
    uint32_t GetSize() const { return size; }

  private:
    uint32_t size;
    Scope<VertexBufferBackend> backend;
};
}