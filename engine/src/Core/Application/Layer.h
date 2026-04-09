#pragma once

namespace Zafkiel 
{

class Layer : public RefCounted
{
  public:
    Layer(const std::string &name) : name(name) {}
    virtual ~Layer() = default;

    virtual void OnAttach() = 0;
    virtual void OnDetach() = 0;
    virtual void OnUpdate(float timestep) = 0;

  protected:
    std::string name;
};

}