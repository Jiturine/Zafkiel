#pragma once

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
    IndexBuffer(const uint32_t* indices, uint32_t count, Scope<IndexBufferBackend> backend)
        : count(count), backend(std::move(backend)) {}
        
    Observer<IndexBufferBackend> GetBackend() { return backend; }
    const Observer<IndexBufferBackend> GetBackend() const { return backend; }

    uint32_t GetCount() const { return count; }

  private:
    Scope<IndexBufferBackend> backend;
    uint32_t count;
};

}