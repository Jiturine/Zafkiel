#pragma once
#include "function/render/texture.h"
#include "function/render/render_pass.h"

namespace Zafkiel
{

struct FrameBufferSpecification
{
    uint32_t width, height;
    std::vector<RenderHandle> attachments;
    RenderHandle renderPass;
};

class FrameBufferBackend
{
  public:
    virtual ~FrameBufferBackend() = default;
};

class FrameBuffer final
{
  public:
    FrameBuffer(const FrameBufferSpecification &spec, Scope<FrameBufferBackend> backend)
        : spec(spec), backend(std::move(backend)) {}

    uint32_t GetWidth() const { return spec.width; }
    uint32_t GetHeight() const { return spec.height; }

    Borrow<FrameBufferBackend> GetBackend() const { return Borrow(backend); }
  
    const FrameBufferSpecification &GetSpecification() const { return spec; }

  private:
    FrameBufferSpecification spec;
    Scope<FrameBufferBackend> backend;

};

}