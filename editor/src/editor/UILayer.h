#pragma once
#include "Core/Application/Layer.h"
#include "Platform/PlatformApplication.h"
#include "Platform/PlatformWindow/PlatformWindow.h"
#include "editor/RenderPass/UIPass.h"
#include "editor/RenderPass/PresentPass.h"

namespace Zafkiel 
{

class UILayer : public Layer
{
  public:
    UILayer() : Layer("Test Layer") {}
    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnUpdate(float timestep) override;

  private:
    Ref<RHIViewport> viewport;

    Scope<UIPass> uiPass;

    std::vector<Ref<Window>> windows;

    static constexpr uint32 maxObjectSize = 500;
};

}