#pragma once

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
    Observer<PipelineBackend> GetPipelineBackend() { return backend; }
    const Observer<PipelineBackend> GetPipelineBackend() const { return backend; }
    
  private:
    Scope<PipelineBackend> backend;
};

}