#pragma once
#include "panel.h"
#include "function/render/texture.h"

namespace Zafkiel
{
class ToolbarPanel : public Panel
{
  public:
    ToolbarPanel();

    virtual void Render() override;

  private:
    Ref<Texture2D> playButton;
    Ref<Texture2D> stopButton;
    bool isPlaying = false;
};
}