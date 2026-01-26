#pragma once
#include "function/render/shader_reflection.h"

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
    Borrow<UniformBufferBackend> GetBackend() const { return Borrow(backend); }
    uint32_t GetSize() const { return size; }
  private:
    Scope<UniformBufferBackend> backend;
    uint32_t size;
};

}