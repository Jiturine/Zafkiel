#pragma once
#include "function/render/render_handle.h"

namespace Zafkiel
{

class IndexBufferBackend 
{
  public:
    virtual ~IndexBufferBackend() = default;
};

class IndexBuffer final
{
  public:
    IndexBuffer(uint32_t count, Scope<IndexBufferBackend> backend)
        : count(count), backend(std::move(backend)) {}

    Borrow<IndexBufferBackend> GetBackend() const { return Borrow(backend); }
    uint32_t GetCount() const { return count; }

  private:
    Scope<IndexBufferBackend> backend;
    uint32_t count;
};

}