#pragma once
#include "shader.h"

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
    VertexBuffer(const float *vertices, uint32_t size, Scope<VertexBufferBackend> backend)
        : backend(std::move(backend)) {}

    Observer<VertexBufferBackend> GetBackend() { return backend; }
    const Observer<VertexBufferBackend> GetBackend() const { return backend; }

  private:
    Scope<VertexBufferBackend> backend;
};
}