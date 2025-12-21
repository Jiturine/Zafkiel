#pragma once
#include "shader_reflection.h"

namespace Zafkiel
{

class UniformBufferBackend
{
  public:
    virtual ~UniformBufferBackend() = default;
};

class UniformBuffer final
{
  public:
    UniformBuffer(uint32_t size, Scope<UniformBufferBackend> backend)
        : size(size), backend(std::move(backend)) {}
    Observer<UniformBufferBackend> GetBackend() { return backend; }
    const Observer<UniformBufferBackend> GetBackend() const { return backend; }
    uint32_t GetSize() const { return size; }
  private:
    Scope<UniformBufferBackend> backend;
    uint32_t size;
};

}