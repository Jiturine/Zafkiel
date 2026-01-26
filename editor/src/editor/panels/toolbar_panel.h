#pragma once
#include "editor/panels/panel.h"
#include "function/render/texture.h"

namespace Zafkiel
{
class ToolbarPanel : public Panel
{
  public:
    ToolbarPanel();

    virtual void Render() override;

  private:
    Scope<Texture2D> playButton;
    Scope<Texture2D> stopButton;
    bool isPlaying = false;
};
}