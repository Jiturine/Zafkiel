#pragma once
#include "Core/Application/Layer.h"

namespace Zafkiel 
{

class FontLayer : public Layer
{
  public:
    FontLayer() : Layer("Font Layer") {}
    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate(float timestep) override;
};

}