#pragma once
#include "editor/Panels/Panel.h"
#include "Function/RHI/RHIResources.h"

namespace Zafkiel
{
class ToolbarPanel : public Panel
{
  public:
    ToolbarPanel();

    virtual void Render() override;

  private:
    Ref<RHITexture> playButton;
    Ref<RHITexture> stopButton;
    bool isPlaying = false;
};
}