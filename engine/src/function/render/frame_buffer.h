#pragma once
#include "texture.h"
#include "render_pass.h"

namespace Zafkiel
{

struct FrameBufferSpecification
{
    uint32_t width, height;
    std::vector<Observer<Image>> attachments;
    Observer<RenderPass> renderPass;
};

class FrameBufferBackend
{
  public:
    virtual ~FrameBufferBackend() = default;
    virtual void Resize(uint32_t width, uint32_t height) = 0;
};

class FrameBuffer final
{
  public:
    FrameBuffer(const FrameBufferSpecification &spec, Scope<FrameBufferBackend> backend)
        : width(spec.width), height(spec.height), backend(std::move(backend)) {}
    uint32_t GetWidth() const { return width; }
    uint32_t GetHeight() const { return height; }

    void Resize(uint32_t width, uint32_t height)
    {
        this->width = width;
        this->height = height;
        backend->Resize(width, height);
    }
    
    Observer<FrameBufferBackend> GetBackend() { return backend; }
    const Observer<FrameBufferBackend> GetBackend() const { return backend; }

  private:
    uint32_t width;
    uint32_t height;
    Scope<FrameBufferBackend> backend;

};

}