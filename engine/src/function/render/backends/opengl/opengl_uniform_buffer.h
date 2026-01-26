#pragma once
#include "function/render/uniform_buffer.h"

namespace Zafkiel 
{

class OpenGLUniformBufferBackend final : public UniformBufferBackend
{
  public:
    OpenGLUniformBufferBackend(uint32_t size, uint32_t rendererID)
        : size(size), rendererID(rendererID) {}
    ~OpenGLUniformBufferBackend();

    void SetData(uint32_t offset, uint32_t size, const void *data) const; 
    
    uint32_t GetRendererID() const { return rendererID; }
    
  private:
    uint32_t size;
    uint32_t rendererID;
};


}