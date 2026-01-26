#pragma once
#include "function/render/render_handle.h"

namespace Zafkiel 
{

enum class PipelineType
{
    Graphics,
    Compute
};

class PipelineBackend 
{
  public:
    virtual ~PipelineBackend() = default;
};

class Pipeline 
{
  public:
    Pipeline(Scope<PipelineBackend> backend) : backend(std::move(backend)) {}
    virtual ~Pipeline() = default;
    virtual PipelineType GetPipelineType() const = 0;

    Borrow<PipelineBackend> GetBackend() const { return Borrow(backend); }
    
  private:
    Scope<PipelineBackend> backend;
};

}