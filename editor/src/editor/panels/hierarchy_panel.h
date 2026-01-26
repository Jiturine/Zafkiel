#pragma once

#include "function/scene/scene.h"
#include "editor/panels/panel.h"

namespace Zafkiel
{
class HierarchyPanel : public Panel
{
  public:
    HierarchyPanel() {}
    virtual void Render() override;
  private:
    void DrawEntityNode(Entity entity);
};
}